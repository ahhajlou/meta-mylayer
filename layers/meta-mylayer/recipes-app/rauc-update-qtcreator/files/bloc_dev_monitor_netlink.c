#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <errno.h>

#include "bloc_dev_monitor.h"

static callback_with_data_t cpp_callback_function = NULL;

typedef struct {
    char action[16];
    char devpath[256];
    char subsystem[32];
    char devname[64];
    char devtype[32];
    char major[8];
    char minor[8];
} uevent_t;

// Parse netlink uevent message
int parse_uevent(const char *buffer, int len, uevent_t *event) {
    memset(event, 0, sizeof(uevent_t));
    
    const char *ptr = buffer;
    const char *end = buffer + len;
    
    // Skip the first part (action@devpath)
    if (ptr < end) {
        const char *at_pos = strchr(ptr, '@');
        if (at_pos) {
            // Extract action
            int action_len = at_pos - ptr;
            if (action_len < sizeof(event->action)) {
                strncpy(event->action, ptr, action_len);
            }
            
            // Extract devpath
            ptr = at_pos + 1;
            const char *null_pos = ptr;
            while (null_pos < end && *null_pos != '\0') null_pos++;
            
            int devpath_len = null_pos - ptr;
            if (devpath_len < sizeof(event->devpath)) {
                strncpy(event->devpath, ptr, devpath_len);
            }
            
            ptr = null_pos + 1;
        }
    }
    
    // Parse key=value pairs
    while (ptr < end) {
        if (*ptr == '\0') {
            ptr++;
            continue;
        }
        
        const char *line_end = ptr;
        while (line_end < end && *line_end != '\0') line_end++;
        
        if (line_end >= end) break;
        
        // Parse key=value
        const char *eq_pos = strchr(ptr, '=');
        if (eq_pos && eq_pos < line_end) {
            int key_len = eq_pos - ptr;
            const char *value = eq_pos + 1;
            int value_len = line_end - value;
            
            if (strncmp(ptr, "SUBSYSTEM", key_len) == 0 && value_len < sizeof(event->subsystem)) {
                strncpy(event->subsystem, value, value_len);
            } else if (strncmp(ptr, "DEVNAME", key_len) == 0 && value_len < sizeof(event->devname)) {
                strncpy(event->devname, value, value_len);
            } else if (strncmp(ptr, "DEVTYPE", key_len) == 0 && value_len < sizeof(event->devtype)) {
                strncpy(event->devtype, value, value_len);
            } else if (strncmp(ptr, "MAJOR", key_len) == 0 && value_len < sizeof(event->major)) {
                strncpy(event->major, value, value_len);
            } else if (strncmp(ptr, "MINOR", key_len) == 0 && value_len < sizeof(event->minor)) {
                strncpy(event->minor, value, value_len);
            }
        }
        
        ptr = line_end + 1;
    }
    
    return 0;
}

// Check if device is removable USB storage
int is_removable_storage(const char *devname) {
    char path[256];
    FILE *f;
    int removable = 0;
    
    // Extract base device name (e.g., sda from sda1)
    char base_dev[64];
    strcpy(base_dev, devname);
    
    // Remove partition number
    char *p = base_dev;
    while (*p && !(*p >= '0' && *p <= '9')) p++;
    *p = '\0';
    
    snprintf(path, sizeof(path), "/sys/block/%s/removable", base_dev);
    f = fopen(path, "r");
    if (f) {
        fscanf(f, "%d", &removable);
        fclose(f);
    }
    
    return removable;
}

// Get filesystem type
char *get_filesystem_type(const char *device_path) {
    static char fstype[32];
    char command[256];
    FILE *fp;
    
    snprintf(command, sizeof(command), "blkid -o value -s TYPE %s 2>/dev/null", device_path);
    fp = popen(command, "r");
    if (fp) {
        if (fgets(fstype, sizeof(fstype), fp)) {
            // Remove newline
            char *nl = strchr(fstype, '\n');
            if (nl) *nl = '\0';
            pclose(fp);
            return fstype;
        }
        pclose(fp);
    }
    
    return NULL;
}

// Auto-mount the device
int auto_mount_device(const char *devname) {
    char device_path[128];
    char mount_point[128];
    char *fstype;
    int ret;
    
    snprintf(device_path, sizeof(device_path), "/dev/%s", devname);
    snprintf(mount_point, sizeof(mount_point), "/mnt/usb_%s", devname);
    
    printf("Attempting to mount %s...\n", device_path);
    
    // Wait a moment for device to be ready
    sleep(1);
    
    // Get filesystem type
    fstype = get_filesystem_type(device_path);
    if (!fstype) {
        printf("Cannot determine filesystem type for %s\n", device_path);
        return -1;
    }
    
    printf("Detected filesystem: %s\n", fstype);
    
    // Create mount point
    if (mkdir(mount_point, 0755) < 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }
    
    // Mount the device
    ret = mount(device_path, mount_point, fstype, MS_NODEV | MS_NOSUID, NULL);
    if (ret < 0) {
        perror("mount");
        rmdir(mount_point);
        return -1;
    }
    
    printf("Successfully mounted %s at %s\n", device_path, mount_point);
    return 0;
}

// Auto-unmount the device
int auto_unmount_device(const char *devname) {
    char mount_point[128];
    int ret;
    
    snprintf(mount_point, sizeof(mount_point), "/mnt/usb_%s", devname);
    
    ret = umount(mount_point);
    if (ret == 0) {
        rmdir(mount_point);
        printf("Unmounted and removed %s\n", mount_point);
    } else {
        perror("umount");
    }
    
    return ret;
}

int dev_mon_main_loop(void) {
    int sock;
    struct sockaddr_nl addr;
    char buffer[4096];
    uevent_t event;
    cpp_callback_function();
    
    sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = 0;
    addr.nl_groups = 1;
    
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }
    
    printf("Monitoring USB device events...\n");
    
    while (1) {
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len < 0) {
            perror("recv");
            continue;
        }
        
        buffer[len] = '\0';
        
        if (parse_uevent(buffer, len, &event) == 0) {
            // Only process block devices
            if (strcmp(event.subsystem, "block") != 0) {
                continue;
            }
            
            printf("Event: %s, Device: %s, DevPath: %s\n", 
                   event.action, event.devname, event.devpath);
            
            if (strlen(event.devname) > 0) {
                if (strcmp(event.action, "add") == 0) {
                    // Check if it's a removable storage device
                    if (is_removable_storage(event.devname)) {
                        printf("Removable storage device added: %s\n", event.devname);
                        cpp_callback_function();
                        
                        // Only auto-mount partitions (not whole devices)
                        if (strchr(event.devname, '1') || strchr(event.devname, '2') || 
                            strchr(event.devname, '3') || strchr(event.devname, '4')) {
                            auto_mount_device(event.devname);
                        }
                    }
                } else if (strcmp(event.action, "remove") == 0) {
                    if (is_removable_storage(event.devname)) {
                        printf("Removable storage device removed: %s\n", event.devname);
                        auto_unmount_device(event.devname);
                    }
                }
            }
        }
    }
    
    close(sock);
    return 0;
}

void register_callback_with_data(callback_with_data_t cb)
{
    cpp_callback_function = cb;
}