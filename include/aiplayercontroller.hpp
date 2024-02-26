// AIPLAYERCONTROLLER.HPP
//  Original - LeAnn Alexandra 2023 -
//  https://github.com/LeannAlexandra/virtualController This version - LeAnn
//  Alexandra 2024 - https://github.com/LeannAlexandra/shinyminer
//  Alexandra 2024 - https://github.com/LeannAlexandra/aiplayer

#ifndef AIPLAYERCONTROLLER_H
#define AIPLAYERCONTROLLER_H

#include <cstddef>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string>
#include <vector>

// THE _A at the end indicates that the values need to be explicitly set. (ie
// not a straightforward button)
#define AIC_DPAD_X_A 0
#define AIC_DPAD_Y_A 1
#define AIC_LANALOG_X_A 2
#define AIC_LANALOG_Y_A 3
#define AIC_RANALOG_X_A 4
#define AIC_RANALOG_Y_A 5
#define AIC_LTRIGGER_A 6
#define AIC_RTRIGGER_A 7
// THE _B at the end indicates straightforward button
#define AIC_X_B 1
#define AIC_Y_B 0
#define AIC_B_B 2
#define AIC_A_B 3
#define AIC_RBUMPER_B 10
#define AIC_LBUMPER_B 9
#define AIC_SELECT_B 5
#define AIC_START_B 6
#define AIC_MODE_B 4
#define AIC_L3_B 7
#define AIC_R3_B 8

///TODO: UPDATE BUTTON LABELS!!! THEY ARE OUT OF ORDER CURENTLY
const std::vector<std::string> button_label = {
    "Y Button",     "X Button",    "B Button",      "A Button",
    "Right Bumper", "Left Bumper", "Select Button", "Start Button",
    "Elon Button",  "L3 Button",   "R3 Button"};

const std::vector<std::string> abs_label = {
    "DPAD X",         "DPAD Y",         "Left Analog X", "Left Analog Y",
    "Right Analog X", "Right Analog Y", "Left Trigger",  "Right Trigger"};

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
  void sendKeyPress(int buttonIndex, int millisecond_duration);
  void cli_control();// use cli to press buttons (calibrate controller) - it is still primitive, but it has potential
  // void setupAbsoluteAxes(int fd_virtual_device); 

private:
  int fd_real_device;
  int fd_virtual_device;
  void sync();
  State controllerState;
  
};
#endif
