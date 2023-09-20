#include <stdio.h>
#include <libudev.h>

int notmain() {
    // Create a udev context
    struct udev *udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Failed to create udev context\n");
        return 1;
    }

    // Create a udev enumerator for input devices
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    // Get a list of devices
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    // Iterate through the list of devices
    udev_list_entry_foreach(entry, devices) {
        const char *syspath = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, syspath);

        // Check if the device has a name attribute
        const char *deviceName = udev_device_get_sysattr_value(dev, "name");
        if (deviceName) {
            printf("Device Name: %s (Event Device: %s)\n", deviceName, udev_device_get_devnode(dev));
        }

        udev_device_unref(dev);
    }

    // Cleanup
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}












// #include <stdio.h>
// #include <stdlib.h>
// #include <dirent.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <libudev.h>
// #include <linux/input.h>

// int isXboxSeriesSController(int fd) {
//     struct input_id device_id;
//     ioctl(fd, EVIOCGID, &device_id);
    
//     // Check if the VID and PID match those of the Xbox Series S controller
//     if (device_id.vendor == 0x045e && device_id.product == 0x02d1) {
//         return 1; // It's an Xbox Series S controller
//     } else {
//         return 0; // It's not an Xbox Series S controller
//     }
// }

// int main() {
//     DIR *dir;
//     struct dirent *entry;

//     dir = opendir("/dev/input");
//     if (dir == NULL) {
//         perror("opendir");
//         return 1;
//     }

//     printf("Event\tPath\tID\n");

//     while ((entry = readdir(dir))) {
//         // Skip entries that are directories or have specific names
//         if (entry->d_type == DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 ||
//             strcmp(entry->d_name, "by-id") == 0 || strcmp(entry->d_name, "by-path") == 0) {
//             continue;
//         }

//         char device_path[256];
//         snprintf(device_path, sizeof(device_path), "/dev/input/%s", entry->d_name);

//         int fd = open(device_path, O_RDONLY | O_NONBLOCK);
//         if (fd != -1) {
//             char device_name[256];
//             ioctl(fd, EVIOCGNAME(sizeof(device_name)), device_name);

//             struct input_id device_id;
//             ioctl(fd, EVIOCGID, &device_id);

//             printf("%s\t%s\t%04x:%04x\n", entry->d_name, device_path, device_id.vendor, device_id.product);

//             if (isXboxSeriesSController(fd)) {
//                 printf("%s\t%s\t%04x:%04x (Xbox Series S Controller)\n", entry->d_name, device_path, 0x045e, 0x02d1);
//             } else {
//                 printf("%s\t%s\t%04x:%04x\n", entry->d_name, device_path, device_id.vendor, device_id.product);
//             }

        
//             close(fd);



//         }
//     }

//     closedir(dir);

//     return 0;
// }
