#include <stdio.h>
#include <libudev.h>

int main() {
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
            printf("Device Name: %s\n", deviceName);
        }

        udev_device_unref(dev);
    }

    // Cleanup
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}
