#ifndef VIRTUALCONTROLLER_H
#define VIRTUALCONTROLLER_H

#include <cstddef>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <vector>

struct State {
    std::vector<bool> buttonStates;    // Store button states
    std::vector<float> absoluteValues; // Store absolute (analog) values

    State(int numButtons, int numAbsValues)
        : buttonStates(numButtons, false), absoluteValues(numAbsValues, 0) {}
  };

class VirtualController {
public:
  VirtualController(bool printDebugInfoToConsole = false);
  ~VirtualController();

  void setButtonState(int buttonIndex, bool state);
  void toggleButtonState(int buttonIndex);
  bool getButtonState(int buttonIndex);
  void setAbsoluteValue(int absIndex, float value);
  float getAbsoluteValue(int absIndex);

  void setControllerState(State newState);
  void setControllerState(); // setstate()  empty = clearState
  void clearControllerState();
  State getControllerState();
  void updateControllerState();

private:
  bool debugging = true;
  int fd_real_device; // Consider moving this inside the class if it's intended
                      // to be a member variable
  int fd_virtual_device;

  std::vector<bool> buttonStates; // Member variable to store button states
                                  // Add any other private members as needed
  void debugPrintState(bool override);
  void sync();
  State controllerState;
  // State clearState;
};
#endif
