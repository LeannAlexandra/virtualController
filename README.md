# virtualController
intercepts input controller to be duplicated to many virtual controllers<br>
<img src="https://github.com/LeannAlexandra/virtualController/blob/main/assets/flow.png?raw=true" alt="flowchart_depiction">

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


