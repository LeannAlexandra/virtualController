// AIPLAYERCONTROLLER.CPP
// original - LeAnn Alexandra 2023 -
// https://github.com/LeannAlexandra/virtualController this version - LeAnn
// Alexandra 2024 - https://github.com/LeannAlexandra/shinyminer
// Alexandra 2024 - https://github.com/LeannAlexandra/aiplayer
#include "../include/aiplayercontroller.hpp"
#include <fcntl.h>
#include <iostream>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// these will never change for XBOX controller.
#define BUTTONS_ON_CONTROLLER 11
#define ABS_VALUES_ON_CONTROLLER 8

const int
    buttonsToEnable[] = {BTN_WEST, BTN_NORTH,  BTN_EAST,   BTN_SOUTH,
                         BTN_TR,   BTN_TL,     BTN_SELECT, BTN_START,
                         BTN_MODE, BTN_THUMBL, BTN_THUMBR}; // 11 constants for
                                                            // xbox controller.
const int floatValuesToEnable[] = {ABS_HAT0X, ABS_HAT0Y, ABS_X, ABS_Y, ABS_RX,
                                   ABS_RY,    ABS_Z,     ABS_RZ}; // 8 values.

AIPlayerController::AIPlayerController()
    : fd_virtual_device(-1),
      controllerState(BUTTONS_ON_CONTROLLER, ABS_VALUES_ON_CONTROLLER) {

  // debugPrintState(true); //doent work as intended // not important eitherr.
  struct uinput_setup setup;
  memset(&setup, 0, sizeof(struct uinput_setup));

  fd_virtual_device = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fd_virtual_device < 0) {
    perror("Error opening virtual input device");
    // Handle the error appropriately, e.g., throw an exception or set an error
    // flag.
  }

  // Enable EV_KEY and EV_SYN event types
  ioctl(fd_virtual_device, UI_SET_EVBIT, EV_KEY);
  ioctl(fd_virtual_device, UI_SET_EVBIT, EV_ABS);
  ioctl(fd_virtual_device, UI_SET_EVBIT, EV_SYN);

  // Enable the buttons specified in buttonsToEnable array
  for (int button : buttonsToEnable) {
    ioctl(fd_virtual_device, UI_SET_KEYBIT, button);
  }

  // setupAbsoluteAxes(int fd_virtual_device)
  for (int button : floatValuesToEnable) {
    struct uinput_abs_setup abs_setup;
    if (button == ABS_HAT0X || button == ABS_HAT0Y) {
      abs_setup.absinfo.maximum = 1;
      abs_setup.absinfo.minimum = -1;
    } else {
      continue;
      // abs_setup.absinfo.maximum = 32767;
      // abs_setup.absinfo.minimum = -32767;
    } // only setup the dpad (the analog is not important for our current
      // adventure)

    ioctl(fd_virtual_device, UI_SET_ABSBIT, button);

    abs_setup.code = button;

    ioctl(fd_virtual_device, UI_ABS_SETUP, &abs_setup);
  }
  // Set the ABS properties for each axis
  // Example for a thumbstick where X and Y are normalized between -1 and 1, and
  // the rest are between 0 and 1

  // Set up the virtual device as needed
  setup.id.bustype = BUS_USB;
  setup.id.vendor = 0x45e;  // Microsoft Vendor ID (modify as needed)
  setup.id.product = 0x2ea; // Product ID (modify as needed)
  snprintf(setup.name, UINPUT_MAX_NAME_SIZE, "AI Player Controller");
  ioctl(fd_virtual_device, UI_DEV_SETUP, &setup);
  ioctl(fd_virtual_device, UI_DEV_CREATE);
  printf("\nController initialised\n");
}

AIPlayerController::~AIPlayerController() {
  if (fd_virtual_device >= 0) {
    ioctl(fd_virtual_device, UI_DEV_DESTROY);
    close(fd_virtual_device);
  }
}

void AIPlayerController::setButtonState(int buttonIndex, bool state = false) {
  if (buttonIndex < 0 || buttonIndex > BUTTONS_ON_CONTROLLER)
    return; /// safety

  controllerState.buttonStates[buttonIndex] = state;
  struct input_event ev_press;
  memset(&ev_press, 0, sizeof(ev_press));
  ev_press.type = EV_KEY;
  ev_press.code = buttonsToEnable[buttonIndex];
  ev_press.value = state;
  write(fd_virtual_device, &ev_press, sizeof(ev_press));

  sync();
}
void AIPlayerController::toggleButtonState(int buttonIndex) {
  printf("buttostate %x\n", getButtonState(buttonIndex));
  setButtonState(buttonIndex, !getButtonState(buttonIndex));
  printf("buttostate %d\n", getButtonState(buttonIndex));
}
bool AIPlayerController::getButtonState(int buttonIndex) {
  return controllerState.buttonStates[buttonIndex];
}
void AIPlayerController::setAbsoluteValue(int absIndex, float value = 0.0f) {
  if (absIndex > ABS_VALUES_ON_CONTROLLER || absIndex < 0) // safety check
    return;                                                // do nothing

  // abs index 0 & 1 are special values (ie int between -1 and 1)

  if (absIndex < 2) {
    if (value > 1) { // cap top value.
      value = 1.0f;
    }
    if (value < -1) {
      value = -1.0f;
    }
    // set state
    controllerState.absoluteValues[absIndex] = value;
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_ABS;
    ev.code = floatValuesToEnable[absIndex];
    ev.value = controllerState.absoluteValues[absIndex];
    write(fd_virtual_device, &ev, sizeof(ev));
  }

  else {
    // all other are float values between min and max (36)
    /// TODO: FIX THIS AWEFUL SHOOTS.
    if (value < 0)
      value = 0.0f;

    controllerState.absoluteValues[absIndex] = value;
    for (int absIndex = 0; absIndex < ABS_VALUES_ON_CONTROLLER; ++absIndex) {
      struct input_event ev;
      memset(&ev, 0, sizeof(ev));
      ev.type = EV_ABS;
      ev.code = floatValuesToEnable[absIndex];
      ev.value = controllerState.absoluteValues[absIndex];
      write(fd_virtual_device, &ev, sizeof(ev));
    }
  }
  sync();
}
float AIPlayerController::getAbsoluteValue(int absIndex) {
  if (absIndex > ABS_VALUES_ON_CONTROLLER || absIndex < 0) // safety check
    return 0.0f;

  return controllerState.absoluteValues[absIndex];
}

void AIPlayerController::setControllerState(State newState) {
  controllerState = newState;
  updateControllerState();
}
void AIPlayerController::setControllerState() { clearControllerState(); }
void AIPlayerController::clearControllerState() {
  controllerState = State(BUTTONS_ON_CONTROLLER, ABS_VALUES_ON_CONTROLLER);
  updateControllerState();
}
State AIPlayerController::getControllerState() { return controllerState; }

void AIPlayerController::updateControllerState() {
  // Loop through buttons and generate input events.
  // Set button states
  for (int buttonIndex = 0; buttonIndex < BUTTONS_ON_CONTROLLER;
       ++buttonIndex) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = buttonsToEnable[buttonIndex];
    ev.value = controllerState.buttonStates[buttonIndex] ? 1 : 0;
    write(fd_virtual_device, &ev, sizeof(ev));
  }

  // Set absolute values (assuming all 8 are needed)
  for (int button : floatValuesToEnable) {
    struct uinput_abs_setup abs_setup;
    ioctl(fd_virtual_device, UI_SET_ABSBIT, button);

    abs_setup.code = button;

    // Set correct min and max values for each axis
    if (button == ABS_HAT0X || button == ABS_HAT0Y) {
      abs_setup.absinfo.minimum = -1;
      abs_setup.absinfo.maximum = 1;
    } else {
      abs_setup.absinfo.minimum = -32767; // Adjust this value as needed
      abs_setup.absinfo.maximum = 32767;  // Adjust this value as needed
    }

    // Set fuzz and flat values if necessary
    abs_setup.absinfo.fuzz = 0;
    abs_setup.absinfo.flat = 0;

    ioctl(fd_virtual_device, UI_ABS_SETUP, &abs_setup);
  }
  sync();
}
void AIPlayerController::sync() {
  // Send a SYN_REPORT event to synchronize the changes
  struct input_event syn_event;
  memset(&syn_event, 0, sizeof(syn_event));
  syn_event.type = EV_SYN;
  syn_event.code = SYN_REPORT;
  syn_event.value = 0;
  write(fd_virtual_device, &syn_event, sizeof(syn_event));
}
// debug:
// cli_control();
void AIPlayerController::sendKeyPress(int key, int millisecond_delay = 150) { // this method can now be used to "hold" a button with milliscond accuracy. 
  setButtonState(key, true);
  usleep(millisecond_delay *
         1000); // uses the delay atop *1000 for milliseconds delay - can be
                // overriden upon send keypres
  setButtonState(key, false);
}
void AIPlayerController::cli_control() {
  int ms_delay = 150;
  while (true) {
    std::string input;
    std::cout << "Enter a command ('qqq' to quit): ";
    std::cin >> input;

    if (input == "qqq") {
      std::cout << "Exiting loop..." << std::endl;
      break;
    } else if (input == "a") {
      sendKeyPress(AIC_A_B);

    } else if (input == "b") {
      sendKeyPress(AIC_B_B);
    } else if (input == "x") {
      sendKeyPress(AIC_X_B);
    } else if (input == "y") {
      sendKeyPress(AIC_Y_B);
    } else if (input == "lb") {
      sendKeyPress(AIC_LBUMPER_B);
    } else if (input == "rb") {
      sendKeyPress(AIC_RBUMPER_B);
    } else if (input == "select") {
      sendKeyPress(AIC_SELECT_B);
    } else if (input == "start") {
      sendKeyPress(AIC_START_B);
    } else if (input == "elon" || input == "mode") {
      sendKeyPress(AIC_MODE_B);
    } else if (input == "up") {
      setAbsoluteValue(AIC_DPAD_Y_A, -1.0f);
      usleep(ms_delay * 100);
      setAbsoluteValue(AIC_DPAD_Y_A, 0.0f);
    } else if (input == "down") {
      setAbsoluteValue(AIC_DPAD_Y_A, 1.0f);
      usleep(ms_delay * 100);
      setAbsoluteValue(AIC_DPAD_Y_A, 0.0f);
    } else if (input == "left") {
      setAbsoluteValue(AIC_DPAD_X_A, -1.0f);
      usleep(ms_delay * 100);
      setAbsoluteValue(AIC_DPAD_X_A, 0.0f);
    } else if (input == "right") {
      setAbsoluteValue(AIC_DPAD_X_A, 1.0f);
      usleep(ms_delay * 100);
      setAbsoluteValue(AIC_DPAD_X_A, 0.0f);

    } else if (input == "l3") {
      sendKeyPress(AIC_L3_B);
    } else if (input == "r3") {
      sendKeyPress(AIC_R3_B);
    } else {
      // Perform tasks based on input
      std::cout << "Performing task for input: " << input << std::endl;
      // Add your task logic here
      // else if (input==""){}
    }
  }
}
///TODO: fix the joysticks & triggers:
// void AIPlayerController::setupAbsoluteAxes(int fd_virtual_device) {
//   for (int button : floatValuesToEnable) {
//     struct uinput_abs_setup abs_setup;
//     ioctl(fd_virtual_device, UI_SET_ABSBIT, button);

//     // Set up the absolute axis
//     abs_setup.code = button;
//     abs_setup.absinfo.maximum = 32767;
//     abs_setup.absinfo.minimum = -32767;

//     if (button == ABS_HAT0X || button == ABS_HAT0Y) { // special case for
//     dpad
//       abs_setup.absinfo.minimum = 0;
//       abs_setup.absinfo.maximum = 1;
//     } else if (button == ABS_Z ||
//                button == ABS_RZ) { // special case for triggers
//       abs_setup.absinfo.minimum = 0;
//     }

//     // Apply the setup
//     ioctl(fd_virtual_device, UI_ABS_SETUP, &abs_setup);
//   }
// }