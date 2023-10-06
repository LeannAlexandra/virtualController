#ifndef AIPLAYERCONTROLLER_H
#define AIPLAYERCONTROLLER_H

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

class AIPlayerController {
public:
  AIPlayerController();
  ~AIPlayerController();

  void setButtonState(int buttonIndex, bool state);
  void toggleButtonState(int buttonIndex);
  bool getButtonState(int buttonIndex);
  void setAbsoluteValue(int absIndex, float value);
  float getAbsoluteValue(int absIndex);
  void setControllerState(State newState);
  void setControllerState(); // setstate empty = clearState
  void clearControllerState();
  State getControllerState();
  void updateControllerState();

private:
  //DEPRECATED bool debugging = false;
  int fd_real_device;
  int fd_virtual_device;
  //DEPRECATED std::vector<bool> buttonStates;
  //DEPRECATED void debugPrintState(bool override);
  void sync();
  State controllerState;
};
#endif
