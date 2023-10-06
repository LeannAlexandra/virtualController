// aiplayercontroller.cpp
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
  for (int button : floatValuesToEnable) {
    struct uinput_abs_setup abs_setup;
    ioctl(fd_virtual_device, UI_SET_ABSBIT, button);


    // ABS_X and ABS_Y range from -1 to 1
    abs_setup.code = button;
    abs_setup.absinfo.maximum = 32767;
    abs_setup.absinfo.minimum = -32768;
    if (button == ABS_HAT0X || button == ABS_HAT0Y) // special case dpad
      abs_setup.absinfo.minimum = -32768;
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
  printf("buttostate %x\n",getButtonState(buttonIndex));
  setButtonState(buttonIndex, !getButtonState(buttonIndex));
  printf("buttostate %d\n",getButtonState(buttonIndex));

}
bool AIPlayerController::getButtonState(int buttonIndex) {
  //if (buttonStates[buttonIndex]) {
    return controllerState.buttonStates[buttonIndex];
  //}
  //return false;
}

void AIPlayerController::setAbsoluteValue(int absIndex, float value = 0.0f) {
  if (absIndex > ABS_VALUES_ON_CONTROLLER || absIndex < 0) // safety check
    return;                                                // do nothing

  // abs index 0 & 1 are special values (ie int between -1 and 1)
  if (value > 1) { // cap top value.
    value = 1.0f;
  }
  if (absIndex < 2) {
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
  } else {
    // all other are float values between 0 and 1
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
  for (int absIndex = 0; absIndex < ABS_VALUES_ON_CONTROLLER; ++absIndex) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_ABS;
    ev.code = floatValuesToEnable[absIndex];
    ev.value = controllerState.absoluteValues[absIndex];
    write(fd_virtual_device, &ev, sizeof(ev));
  }
  sync();
}
void AIPlayerController::sync(){
  // Send a SYN_REPORT event to synchronize the changes
  struct input_event syn_event;
  memset(&syn_event, 0, sizeof(syn_event));
  syn_event.type = EV_SYN;
  syn_event.code = SYN_REPORT;
  syn_event.value = 0;
  write(fd_virtual_device, &syn_event, sizeof(syn_event));
}