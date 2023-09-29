# virtualController
<<<<<<< HEAD
<img src="/res/controller-1784573_640.png">
new version
simulates a new device as if a usb was plugged in (obviously for my purpose I did not actually have to spoof a usb device)
<img src="/res/usb-7036075_640.jpg">
but I do make a device that registers a completely operational controller for my purposes I wanted this code to be compatible with output to a hardware controller, thus instead of sending key events directly, it is a complete freestanding virtual controller that is compatible with 11 indixes for buttons and 8 abs values for things like joysticks and controls, these can be maped to match any controller, but since my application is based on enumerators. The biggest reason not to, is because any emulator creates a virtual controller automatically. 


<h2>What can I use it for?</h2>
<img src="/res/nintendo-4659315_640.png">
Giving a "restricted controller" to a machine learning algoritm that can play games - that way the ai can play and learn and survive without jailbreaking the computer. 


old version intercepts input controller to be duplicated to many virtual controllers<br>
![image](https://github.com/LeannAlexandra/virtualController/assets/81232303/2b51e0b0-1b39-44a9-a63f-9f400132085d)

<h2>NEW UPDATE:</h2>
Update now has virtualController.cpp & virtualController.hpp - these can be used as a library- 
<h4>What does virtualController do now?</h4>
virtualController is currently working only on Linux, it registers an event in the linux kernel that closely mimics an xbox controller to be used in code.
in essence giving you a virtual controller. - Since I plan to use this with a neural network ai and give it a couple of buttons to smash (6 to 10 in case of nintendo) I have achieved this perfectly.



<h2>below is the outline of the original project:</h2>
=======
intercepts input controller to be duplicated to many virtual controllers<br>
<img src="https://github.com/LeannAlexandra/virtualController/blob/main/assets/flow.png?raw=true" alt="flowchart_depiction">
>>>>>>> 70c48324b553c85860bcd5a4ce65c80cc370c860

<h2>USE CASE:</h2>
<ul>
<li>play multiple instances emulator games at the same time </li>
  <img src="https://github.com/LeannAlexandra/virtualController/blob/main/assets/mGBAexample.png?raw=true" alt="example_usage">
  <li>use as a foundation for interesting XOR (and or) NAND controlling (when playing same game with a friend, this would requires 2 inputs and some extra coding, but it is possible:D ) </li>
<li>What I really want to do is give control to an ai to do the same as above ;) - but each virtual controller will have additional inputs from ai ;D </li>
</ul>

<h2>How it works</h2>
 <img src="https://github.com/LeannAlexandra/virtualController/blob/main/assets/virtualcontroler_evtest.png?raw=true" alt="evtest proof">
  <em>It just works</em> - Todd Howard<br>
In short: replicates my xbox controller in linux to register multiple virtual controllers.
 <img src="https://github.com/LeannAlexandra/virtualController/blob/main/assets/evtestSync.gif?raw=true" alt="evtest example sync">

<h2>Known Issues - for the one redditor who came on over</h2>

<ul><li>the eventX has to be set manually using evtest, set to the config file</li>
<li>the eventX gets consumed by the emulator if you run emulator before my app. (this is emulator code, it cannot be freed even when closing the emulator.</li>
<li>the known method to dynamically find X (has been deprecated) as it wasn't working, remnants of the output was logged in res/cripnote (it's a text file) </li></ul> 


