/*
 * RAUC GDBus Client
 * Command-line tool for interacting with RAUC via D-Bus
 */


#include "rauc_gdbus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <gio/gio.h>

static GMainLoop *main_loop = NULL;
static gboolean verbose = FALSE;
static gboolean monitor_mode = FALSE;

// Signal callback for RAUC Completed
static void on_rauc_completed(GDBusConnection *connection,
                             const gchar *sender_name,
                             const gchar *object_path,
                             const gchar *interface_name,
                             const gchar *signal_name,
                             GVariant *parameters,
                             gpointer user_data) {

    if (g_strcmp0(signal_name, "Completed") == 0) {
        gint32 result;
        g_variant_get(parameters, "(i)", &result);

        if (verbose) {
            printf("Signal received: RAUC Installation Completed\n");
        }
        printf("Installation %s (result code: %d)\n",
               result == 0 ? "SUCCESS" : "FAILED", result);

        if (!monitor_mode) {
            g_main_loop_quit(main_loop);
        }
    }
}

// Properties changed callback for progress updates
static void on_properties_changed(GDBusConnection *connection,
                                 const gchar *sender_name,
                                 const gchar *object_path,
                                 const gchar *interface_name,
                                 const gchar *signal_name,
                                 GVariant *parameters,
                                 gpointer user_data) {

    const gchar *interface;
    GVariant *changed_properties;
    GVariant *progress_variant;

    g_variant_get(parameters, "(&s@a{sv}^as)", &interface, &changed_properties, NULL);

    if (g_strcmp0(interface, "de.pengutronix.rauc.Installer") == 0) {
        progress_variant = g_variant_lookup_value(changed_properties, "Progress", NULL);
        if (progress_variant) {
            gint32 percentage, nesting_depth;
            const gchar *message;

            g_variant_get(progress_variant, "(i&si)", &percentage, &message, &nesting_depth);
            printf("\rProgress: %3d%% - %s", percentage, message);
            fflush(stdout);

            if (percentage == 100) {
                printf("\n");
            }

            g_variant_unref(progress_variant);
        }

        // Check for Operation property changes
        GVariant *operation_variant = g_variant_lookup_value(changed_properties, "Operation", NULL);
        if (operation_variant) {
            const gchar *operation;
            g_variant_get(operation_variant, "&s", &operation);

            if (verbose) {
                printf("Operation: %s\n", operation);
            }

            g_variant_unref(operation_variant);
        }
    }

    g_variant_unref(changed_properties);
}

// Get bundle information
int cmd_info(const char* bundle_path) {
    GError *error = NULL;
    GDBusConnection *connection;
    GVariant *result;
    const gchar *compatible, *version;

    printf("Getting bundle information for: %s\n", bundle_path);

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error != NULL) {
        fprintf(stderr, "Error: Failed to connect to D-Bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    result = g_dbus_connection_call_sync(
        connection,
        "de.pengutronix.rauc",
        "/",
        "de.pengutronix.rauc.Installer",
        "Info",
        g_variant_new("(s)", bundle_path),
        G_VARIANT_TYPE("(ss)"),
        G_DBUS_CALL_FLAGS_NONE,
        10000, // 10 second timeout
        NULL,
        &error
    );

    if (error != NULL) {
        fprintf(stderr, "Error: Failed to get bundle info: %s\n", error->message);
        g_error_free(error);
        g_object_unref(connection);
        return 1;
    }

    g_variant_get(result, "(&s&s)", &compatible, &version);
    printf("Bundle Information:\n");
    printf("  Compatible: %s\n", compatible);
    printf("  Version:    %s\n", version);

    g_variant_unref(result);
    g_object_unref(connection);
    return 0;
}

// Install bundle
int cmd_install(const char* bundle_path) {
    GError *error = NULL;
    GDBusConnection *connection;

    printf("Installing bundle: %s\n", bundle_path);

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error != NULL) {
        fprintf(stderr, "Error: Failed to connect to D-Bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Subscribe to completion signal
    g_dbus_connection_signal_subscribe(
        connection,
        "de.pengutronix.rauc",
        "de.pengutronix.rauc.Installer",
        "Completed",
        "/",
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_rauc_completed,
        NULL,
        NULL
    );

    // Subscribe to progress updates
    g_dbus_connection_signal_subscribe(
        connection,
        "de.pengutronix.rauc",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        "/",
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_properties_changed,
        NULL,
        NULL
    );

    // Start installation
    g_dbus_connection_call(
        connection,
        "de.pengutronix.rauc",
        "/",
        "de.pengutronix.rauc.Installer",
        "Install",
        g_variant_new("(s)", bundle_path),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        NULL, // No callback needed for async call
        NULL
    );

    printf("Installation started, waiting for completion...\n");

    // Run main loop to wait for signals
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);

    g_object_unref(connection);
    return 0;
}

// Get slot status
int cmd_status(void) {
    GError *error = NULL;
    GDBusConnection *connection;
    GVariant *result;
    GVariantIter iter;
    GVariant *slot_data;

    printf("Getting slot status...\n");

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error != NULL) {
        fprintf(stderr, "Error: Failed to connect to D-Bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    result = g_dbus_connection_call_sync(
        connection,
        "de.pengutronix.rauc",
        "/",
        "de.pengutronix.rauc.Installer",
        "GetSlotStatus",
        NULL,
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        10000,
        NULL,
        &error
    );

    if (error != NULL) {
        fprintf(stderr, "Error: Failed to get slot status: %s\n", error->message);
        g_error_free(error);
        g_object_unref(connection);
        return 1;
    }

    printf("Slot Status:\n");
    g_variant_iter_init(&iter, g_variant_get_child_value(result, 0));

    while ((slot_data = g_variant_iter_next_value(&iter)) != NULL) {
        const gchar *slot_name;
        GVariant *properties;

        g_variant_get(slot_data, "(&s@a{sv})", &slot_name, &properties);
        printf("  %s:\n", slot_name);

        // Print slot properties
        GVariantIter prop_iter;
        g_variant_iter_init(&prop_iter, properties);
        const gchar *key;
        GVariant *value;

        while (g_variant_iter_loop(&prop_iter, "{&sv}", &key, &value)) {
            gchar *value_str = g_variant_print(value, FALSE);
            printf("    %s: %s\n", key, value_str);
            g_free(value_str);
        }

        g_variant_unref(properties);
        g_variant_unref(slot_data);
    }

    g_variant_unref(result);
    g_object_unref(connection);
    return 0;
}

// Monitor RAUC signals
int cmd_monitor(void) {
    GError *error = NULL;
    GDBusConnection *connection;

    monitor_mode = TRUE;
    printf("Monitoring RAUC signals... (Ctrl+C to stop)\n");

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error != NULL) {
        fprintf(stderr, "Error: Failed to connect to D-Bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Subscribe to all RAUC signals
    g_dbus_connection_signal_subscribe(
        connection,
        "de.pengutronix.rauc",
        "de.pengutronix.rauc.Installer",
        NULL, // All signals
        "/",
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_rauc_completed,
        NULL,
        NULL
    );

    g_dbus_connection_signal_subscribe(
        connection,
        "de.pengutronix.rauc",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        "/",
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_properties_changed,
        NULL,
        NULL
    );

    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);

    g_object_unref(connection);
    return 0;
}

/*
// Print help
static void print_help(const char* program_name) {
    printf("Usage: %s [OPTIONS] COMMAND [ARGS]\n\n", program_name);
    printf("Commands:\n");
    printf("  info BUNDLE      Get bundle information\n");
    printf("  install BUNDLE   Install bundle\n");
    printf("  status          Show slot status\n");
    printf("  monitor         Monitor RAUC signals\n");
    printf("\nOptions:\n");
    printf("  -v, --verbose   Enable verbose output\n");
    printf("  -h, --help      Show this help\n");
}*/

/*
int main(int argc, char *argv[]) {
    int opt;
    const char *command;

    static struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Parse options
    while ((opt = getopt_long(argc, argv, "vh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v':
                verbose = TRUE;
                break;
            case 'h':
                print_help(argv[0]);
                return 0;
            default:
                print_help(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: No command specified\n");
        print_help(argv[0]);
        return 1;
    }

    command = argv[optind];

    if (strcmp(command, "info") == 0) {
        if (optind + 1 >= argc) {
            fprintf(stderr, "Error: Bundle path required for info command\n");
            return 1;
        }
        return cmd_info(argv[optind + 1]);
    }
    else if (strcmp(command, "install") == 0) {
        if (optind + 1 >= argc) {
            fprintf(stderr, "Error: Bundle path required for install command\n");
            return 1;
        }
        return cmd_install(argv[optind + 1]);
    }
    else if (strcmp(command, "status") == 0) {
        return cmd_status();
    }
    else if (strcmp(command, "monitor") == 0) {
        return cmd_monitor();
    }
    else {
        fprintf(stderr, "Error: Unknown command '%s'\n", command);
        print_help(argv[0]);
        return 1;
    }
}*/
