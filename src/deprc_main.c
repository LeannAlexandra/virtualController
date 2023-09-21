//main.c
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/controller.h"

// Function declaration for finding the event number
int findEventNumber(const char *deviceName, char *eventNumber,
                    size_t eventNumberSize);


int notAmain() {
  // Open and read the configuration file
  FILE *file = fopen("conf/config.conf", "r");
  if (file == NULL) {
    perror("Error opening config file");
    return 1;
  }

  // Initialize variables to store configuration values
  char device[256];
  int virtualControllers = 0;
  char eventNumber[8]; // Adjust the size as needed

  // Read and parse the configuration lines
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    // Remove trailing newline character
    line[strcspn(line, "\n")] = '\0';

    // Split the line into key and value
    char *key = strtok(line, "=");
    char *value = strtok(NULL, "=");

    if (key && value) {
      if (strcmp(key, "device") == 0) {
        strncpy(device, value, sizeof(device));
      } else if (strcmp(key, "virtual_controllers") == 0) {
        virtualControllers = atoi(value);
      }
    }
  }
  // Close the configuration file
  fclose(file);

  // Print the parsed values
  printf("Device: %s\n", device);
  printf("Virtual Controllers: %d\n", virtualControllers);

  // Call the function to find the event number
  if (findEventNumber(device, eventNumber, sizeof(eventNumber)) != 0) {
    // Handle the case where the device is not found
    // ...
    return 1; // Return an error code
  }

  printf("DEVICE's eventNumber from udev_info.c:   %s   \n", eventNumber);
  // Use the event number to open the real input device
  char devicePath[20];
  snprintf(devicePath, sizeof(devicePath), "/dev/input/event%s", eventNumber);
  int fd_real_device = open(devicePath, O_RDONLY | O_NONBLOCK);

  if (fd_real_device < 0) {
    perror("Error opening real input device");
    return 1;
  }

  Controller clones[virtualControllers];
  Controller templateController;
    createTemplateController(templateController); // Get the template

  // Duplicate the template to create clones
  for (int i = 0; i < virtualControllers; i++) {
    duplicateController(templateController,clones[i]);
  }

  // Open virtual input devices (you'll need to create 4 of these)
  int fd_virtual_device1 = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  // ...

  // Set up virtual devices (initialize them)
  // ...

  // Create a uinput event structure to hold input events
  struct input_event ev;

  while (1) {
    // Capture input events from the real device
    ssize_t n = read(fd_real_device, &ev, sizeof(ev));
    if (n == sizeof(ev)) {
      // Replicate the input event to all virtual devices
      write(fd_virtual_device1, &ev, sizeof(ev));
      // ...
    }

    // Handle input event synchronization if needed
    // ...
  }

  // Close all open file descriptors and clean up
  // ...
  for (int i = 0; i < virtualControllers; i++) {
    cleanupController(&clones[i]);
  }
  return 0;
}
