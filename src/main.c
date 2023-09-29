#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define NUM_VIRTUAL_CONTROLLERS 4

int virtualControllers = 0;
int deviceNumber = -1;             // force error if unassigned
int continuousPollingDelay = 2000; // microseconds ie 2ms
int fd_real_device;
// Create and configure virtual input devices
int fd_virtual_devices[NUM_VIRTUAL_CONTROLLERS];
int onlyCleanOnce=0;

void cleanup() {
  if(onlyCleanOnce++>0)
    return;
  printf("\n\nCLEANUP ACTIVATED");
  // Destroy all virtual devices
  for (int i = 0; i < NUM_VIRTUAL_CONTROLLERS; i++) {
    ioctl(fd_virtual_devices[i], UI_DEV_DESTROY);
    close(fd_virtual_devices[i]);
  }

  // Release the "grab" on the original input device when the program exits
  ioctl(fd_real_device, EVIOCGRAB, 0);
  close(fd_real_device);

  printf("\nSqueeky Clean,\nThank you for using virtual Controllers\n\t\tLeAnn Alexandra \u00A9 2023\n");
  // exit from here?
  exit(0);
}

void updateConfigFile() {
  FILE *configFile = fopen("conf/config.conf", "w");
  if (configFile == NULL) {
    perror("Error opening config file for writing");
    exit(EXIT_FAILURE);
  }

  fprintf(configFile, "device=%d\n", deviceNumber);
  fprintf(configFile, "virtual_controllers=%d\n", virtualControllers);
  fprintf(configFile, "continuous_polling_delay=%d\n", continuousPollingDelay);

  fclose(configFile);
}

int main(int argc, char *argv[]) {
  int opt;
  int c_flag = 0;
  int i_flag = 0;
  int d_flag = 0;
  int p_flag = 0;
  int h_flag = 0;

  // Define the help message
  const char *help_message =
      "Usage: ./virtualController [OPTIONS]\n"
      "Options:\n"
      "  -c, --controllers <num>  Set the number of virtual controllers\n"
      "  -i, --interval <ms>      Set the continuous polling delay in "
      "milliseconds\n"
      "  -d, --device <num>       Set the device number (e.g., -d 11)\n"
      "  -p, --persist            Save the new configuration to the config "
      "file\n"
      "  -h, --help               Show this help message\n";

  // Use getopt to parse command-line arguments
  while ((opt = getopt(argc, argv, "c:i:d:ph")) != -1) {
    switch (opt) {
    case 'c':
      virtualControllers = atoi(optarg);
      c_flag = 1;
      break;
    case 'i':
      continuousPollingDelay = atoi(optarg);
      i_flag = 1;
      break;
    case 'd':
      deviceNumber = atoi(optarg);
      d_flag = 1;
      break;
    case 'p':
      p_flag = 1;
      break;
    case 'h':
      h_flag = 1;
      break;
    default:
      fprintf(stderr, "Invalid option\n");
      h_flag = 1;
      break;
    }
  }

  // If -h or --help flag is present, print help message and exit
  if (h_flag) {
    printf("%s", help_message);
    exit(EXIT_SUCCESS);
  }

  // Read values from the configuration file
  FILE *configFile = fopen("conf/config.conf", "r");
  if (configFile != NULL) {
    char line[256];
    while (fgets(line, sizeof(line), configFile)) {
      // Remove trailing newline character
      line[strcspn(line, "\n")] = '\0';
//jhgjhfgkhghgfku
      // Split the line into key and value
      char *key = strtok(line, "=");
      char *value = strtok(NULL, "=");

      if (key && value) {
        if (strcmp(key, "device") == 0 && !d_flag) {
          deviceNumber = atoi(value);
        } else if (strcmp(key, "virtual_controllers") == 0 && !c_flag) {
          virtualControllers = atoi(value);
        } else if (strcmp(key, "continuous_polling_delay") == 0 && !i_flag) {
          continuousPollingDelay = atoi(value);
        }
      }
    }

    fclose(configFile);
  }

  printf("Device Number: %d\n", deviceNumber);
  printf("Virtual Controllers: %d\n", virtualControllers);
  printf("Continuous Polling Delay: %d\n", continuousPollingDelay);

  if (p_flag) {
    // If -p option is provided, update the configuration file with new values
    updateConfigFile();
    printf("Configuration updated and saved.\n");
  }

  if (deviceNumber < 0) {
    perror("DEVICE NUMBER NOT SET IN .conf, or .conf is missing\n");
    return 1;
  }

  // Open the original input device (event7) for reading
  fd_real_device = open("/dev/input/event7", O_RDONLY | O_NONBLOCK);
  if (fd_real_device < 0) {
    perror("Error opening real input device - consider seeing if it is "
           "connected or set up correctly in config.conf by using `evtest`");
    return 1;
  }
  // Use EVIOCGRAB to "grab" the original input device
  if (ioctl(fd_real_device, EVIOCGRAB, 1) < 0) {
    perror("Error grabbing real input device - abort the mission");
    return 1;
  }

  // Set up a signal handler to release the grab when the program is terminated
  atexit(cleanup);
  printf("Setting up signal handler...\n");
  signal(SIGINT, cleanup); // Release grab on Ctrl+C
  printf("Signal handler set up.\n");
  sigset_t sigset;
  sigprocmask(0, NULL, &sigset);
  if (sigismember(&sigset, SIGINT)) {
    printf("SIGINT is masked.\n");
  }

  // Create and configure virtual input devices
  struct uinput_setup setup;
  memset(&setup, 0, sizeof(struct uinput_setup));

  for (int i = 0; i < NUM_VIRTUAL_CONTROLLERS; i++) {
    // Open virtual input devices for writing
    fd_virtual_devices[i] = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd_virtual_devices[i] < 0) {
      perror("Error opening virtual input device - abort the mission");
      return 1;
    }

    ////////////////BUTTONS/////////////// type 1
    // Configure virtual input devices
    ioctl(fd_virtual_devices[i], UI_SET_EVBIT,
          EV_ABS); // Enable ABS events (type3)
    ioctl(fd_virtual_devices[i], UI_SET_EVBIT, EV_KEY);
    ioctl(fd_virtual_devices[i], UI_SET_EVBIT, EV_SYN);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_A);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_B);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_X);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_Y);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_TR);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_TL);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_SELECT);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_START);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_MODE);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_THUMBL);
    ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_THUMBR);
    /// values type 3:
    // Add ABS event codes
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_X);
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_Y);
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_RX);
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_RY);
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_Z);
    ioctl(fd_virtual_devices[i], UI_SET_ABSBIT, ABS_RZ);

    // Set up the virtual device as needed
    setup.id.bustype = BUS_USB;
    setup.id.vendor = 0x45e;  // Microsoft Vendor ID (modify as needed)
    setup.id.product = 0x2ea; // Product ID (modify as needed)
    snprintf(setup.name, UINPUT_MAX_NAME_SIZE, "aiPlayer %d", i);
    ioctl(fd_virtual_devices[i], UI_DEV_SETUP, &setup);
    ioctl(fd_virtual_devices[i], UI_DEV_CREATE);
  }

  // Create a uinput event structure to hold input events
  struct input_event ev;

  while (1) {
    // Capture input events from the real device
    ssize_t n = read(fd_real_device, &ev, sizeof(ev));
    if (n == sizeof(ev)) {
      // Replicate the input event to all virtual devices
      for (int i = 0; i < virtualControllers; i++) {
        write(fd_virtual_devices[i], &ev, sizeof(ev));
      }
    }
    // Add a sleep of 2 milliseconds (2000 microseconds)
    usleep(2000);

    // Handle input event synchronization or other logic if needed
    // ...
  }
  //backup code (from cleanup)
  // Clean up and close all open file descriptors
  for (int i = 0; i < virtualControllers; i++) {
    ioctl(fd_virtual_devices[i], UI_DEV_DESTROY);
    close(fd_virtual_devices[i]);
  }
  // Release the "grab" on the original input device
  ioctl(fd_real_device, EVIOCGRAB, 0);
  close(fd_real_device);
  
  return 0;
}