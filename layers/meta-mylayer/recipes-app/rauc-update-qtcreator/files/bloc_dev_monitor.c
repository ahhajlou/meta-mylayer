#include <libudev.h>
#include <stdio.h>

#include "bloc_dev_monitor.h"

static callback_with_data_t cpp_callback_function = NULL;

int udev_dev_mon_main_loop() {
    printf("STARING TO MONITOR BLOC DEVICES\n");
    struct udev *udev;
    struct udev_monitor *mon;
    
    udev = udev_new();
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
    udev_monitor_enable_receiving(mon);
    
    printf("Blocking on udev events...\n");
    
    while (1) {
        // This blocks until an event occurs - NO POLLING!
        struct udev_device *dev = udev_monitor_receive_device(mon);
        if (dev) {
            const char *action = udev_device_get_action(dev);
            printf("Event: %s\n", action);
            
            // Process the device...
            
            udev_device_unref(dev);
        }
    }
    
    return 0;
}

void register_callback_with_data(callback_with_data_t cb)
{
    cpp_callback_function = cb;
}