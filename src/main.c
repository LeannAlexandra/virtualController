#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_VIRTUAL_CONTROLLERS 4

int main() {
    // Open the original input device (event11) for reading
    int fd_real_device = open("/dev/input/event11", O_RDONLY | O_NONBLOCK);
    if (fd_real_device < 0) {
        perror("Error opening real input device - consider seeing if it is connected or set up correctly in config.conf by using `evtest`");
        return 1;
    }
 // Use EVIOCGRAB to "grab" the original input device
    if (ioctl(fd_real_device, EVIOCGRAB, 1) < 0) {
        perror("Error grabbing real input device - abort the mission");
        return 1;
    }
    // Create and configure virtual input devices
    int fd_virtual_devices[NUM_VIRTUAL_CONTROLLERS];
    struct uinput_setup setup;
    memset(&setup, 0, sizeof(struct uinput_setup));

    for (int i = 0; i < NUM_VIRTUAL_CONTROLLERS; i++) {
        // Open virtual input devices for writing
        fd_virtual_devices[i] = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (fd_virtual_devices[i] < 0) {
            perror("Error opening virtual input device - abort the mission");
            return 1;
        }

        // Configure virtual input devices
        ioctl(fd_virtual_devices[i], UI_SET_EVBIT, EV_KEY);
        ioctl(fd_virtual_devices[i], UI_SET_EVBIT, EV_SYN);
        // Add more event types and codes as needed
        ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_A);
        ioctl(fd_virtual_devices[i], UI_SET_KEYBIT, BTN_B);

        // Set up the virtual device as needed
        setup.id.bustype = BUS_USB;
        setup.id.vendor = 0x1234;  // Vendor ID (modify as needed)
        setup.id.product = 0x5678; // Product ID (modify as needed)
        snprintf(setup.name, UINPUT_MAX_NAME_SIZE, "Virtual Controller %d", i);
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
            for (int i = 0; i < NUM_VIRTUAL_CONTROLLERS; i++) {
                write(fd_virtual_devices[i], &ev, sizeof(ev));
            }
        }
        // Add a sleep of 2 milliseconds (2000 microseconds)
        usleep(2000);




        // Handle input event synchronization or other logic if needed
        // ...
    }

    // Clean up and close all open file descriptors
    for (int i = 0; i < NUM_VIRTUAL_CONTROLLERS; i++) {
        ioctl(fd_virtual_devices[i], UI_DEV_DESTROY);
        close(fd_virtual_devices[i]);
    }
// Release the "grab" on the original input device
    ioctl(fd_real_device, EVIOCGRAB, 0);

    close(fd_real_device);
    return 0;
}
