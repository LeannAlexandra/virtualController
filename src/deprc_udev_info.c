// udev_info.c
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// udev_info.c
#include "../include/controller.h"

Controller createTemplateController() {
  Controller templateController;
  // Initialize templateController with default configuration
  // ...

  return templateController; /// send this to main ? using it as if a class? and people say C doesn't have class... ;)
}

// Function to print detailed information about a udev device
void printDeviceInfo(struct udev_device *dev, ...) {
  if (!dev) {
    fprintf(stderr, "Device is NULL\n");
    return;
  }

  // Print common attributes
  printf("Device Name: %s\n", udev_device_get_sysattr_value(dev, "name"));
  printf("Device Path: %s\n", udev_device_get_devnode(dev));
  printf("Device ID: %s\n", udev_device_get_property_value(dev, "ID_INPUT"));

  // Add more information as needed

  // Example: Print capabilities
  struct udev_list_entry *capabilities =
      udev_device_get_properties_list_entry(dev);
  struct udev_list_entry *entry;
  printf("Capabilities:\n");
  udev_list_entry_foreach(entry, capabilities) {
    const char *name = udev_list_entry_get_name(entry);
    const char *value = udev_list_entry_get_value(entry);
    printf("Capabilities:\n");
    udev_list_entry_foreach(entry, capabilities) {
      const char *name = udev_list_entry_get_name(entry);
      const char *value = udev_list_entry_get_value(entry);

      printf("  %s=%s\n", name, value);
    }
    printf("  %s=%s\n", name, value);
  }
}
// char* findDevPath(){}
// Function to find and return the event number for a given device name
int findEventNumber(struct udev_device *dev, char *eventNumber,
                    size_t eventNumberSize) {
  // Check if the device has a name attribute
  const char *deviceName = udev_device_get_sysattr_value(dev, "name");
  char* devPath;
  // printf("Checking device: %s\n", deviceName);
  printDeviceInfo(dev, &devPath);

  if (deviceName && strcmp(deviceName, deviceName) == 0) {
    // Found a matching device
    printf("WE FOUND THE DEVICE YOU WERE LOOKING FOR!!! YAY!!! \n\n");

    // Get the event number
    const char *devnode = udev_device_get_devnode(dev);
    if (devnode && strstr(devnode, "/dev/input/event") == devnode) {
      strncpy(eventNumber, devnode + strlen("/dev/input/event"),
              eventNumberSize);
      eventNumber[eventNumberSize - 1] = '\0';
    }

    return 0;
  }

  return -1; // Device not found
}

// #include <libudev.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// // Function to find and return the event number for a given device name
// int findEventNumber(const char *deviceName, char *eventNumber,
//                     size_t eventNumberSize) {
//   // Create a udev context
//   struct udev *udev = udev_new();
//   if (!udev) {
//     fprintf(stderr, "Failed to create udev context\n");
//     return -1;
//   }

//   // Create a udev enumerator for input devices
//   struct udev_enumerate *enumerate = udev_enumerate_new(udev);
//   udev_enumerate_add_match_subsystem(enumerate, "input");
//   udev_enumerate_scan_devices(enumerate);

//   // Get a list of devices
//   struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
//   struct udev_list_entry *entry;

//   // Flag to indicate if a matching device is found
//   int found = 0;

//   // Iterate through the list of devices
//   udev_list_entry_foreach(entry, devices) {
//     const char *syspath = udev_list_entry_get_name(entry);
//     struct udev_device *dev = udev_device_new_from_syspath(udev, syspath);

//         // Check if the device has a name attribute
//         const char *currentDeviceName = udev_device_get_sysattr_value(dev,
//         "name"); printf("Checking device: %s\n", currentDeviceName);
//         printDeviceInfo(dev);
//         if (currentDeviceName && strcmp(currentDeviceName, deviceName) == 0)
//         {
//         // Found a matching device
//         printDeviceInfo(dev);
//         found = 1;
//         printf("WE FOUND THE DEVICE YOU WERE LOOKING FOR!!! YAY!!! \n\n");
//       // Get the event number
//       const char *devnode = udev_device_get_devnode(dev);
//       printf("Device Node: %s\n", devnode);
//       if (devnode && strstr(devnode, "/dev/input/event") == devnode) {
//         strncpy(eventNumber, devnode + strlen("/dev/input/event"),
//                 eventNumberSize);
//         eventNumber[eventNumberSize - 1] = '\0';
//       }

//       break; // Exit the loop when a match is found
//     }

//     udev_device_unref(dev);
//   }

//   if (!found) {
//     fprintf(stderr, "%s not found! Did you mean one of these devices?\n",
//             deviceName);

//     udev_list_entry_foreach(entry, devices) {

//       const char *syspath = udev_list_entry_get_name(entry);
//       struct udev_device *dev = udev_device_new_from_syspath(udev, syspath);

//       // Check if the device has a name attribute
//       const char *currentDeviceName =
//           udev_device_get_sysattr_value(dev, "name");
//       printf("REPORTING device: %s\n", currentDeviceName);
//       if (currentDeviceName && strcmp(currentDeviceName, deviceName) == 0) {
//         // Found a matching device
//         found = 1;
//         printf("Device Name: %s\n", deviceName);

//         // You can also print other information about the device here
//         printf("Device Path: %s\n", udev_device_get_devnode(dev));
//         printf("Device ID: %s\n",
//                udev_device_get_property_value(dev, "ID_INPUT"));
//         // Add more information as needed

//         break; // Exit the loop when a match is found
//       }
//     }

//     // Print a list of available devices here (similar to your original
//     output) return -1;
//   }

//   // Cleanup
//   udev_enumerate_unref(enumerate);
//   udev_unref(udev);
//   return 0;
// }