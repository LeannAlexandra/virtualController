//main.cpp
//THIS IS AN EXAMPLE OF THE IMPLEMENTATION OF THE VIRTUAL CONTROLLER/AIPLAYERCONTROLLER
#include "../include/aiplayercontroller.hpp"

int main (){
    AIPlayerController aic;
    aic.sendKeyPress (AIC_A_B); // Press the A button for duration of 150 milliseconds.
    aic.sendKeyPress (AIC_A_B, 400); // Holds the A button. for 400 millisecond duration
    aic.cli_control(); //allows the virtual controller to be  
    
    return 0;
}