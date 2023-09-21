# virtualController
intercepts input controller to be duplicated to many virtual controllers<br>
![image](https://github.com/LeannAlexandra/virtualController/assets/81232303/2b51e0b0-1b39-44a9-a63f-9f400132085d)


<h2>USE CASE:</h2>
![image](https://github.com/LeannAlexandra/virtualController/assets/81232303/44b34521-f19e-4c6f-9a33-f0c5ab0758e0)

<ul>
<li>play multiple instances emulator games at the same time </li>
<li>use as a foundation for interesting XOR (and or) NAND controlling (when playing same game with a friend , requires 2 inputs) </li>
<li>What I really want to do is give control to an ai to do the same as above ;) - but each virtual controller will have additional inputs from ai ;D </li>
</ul>

<h2>How it works</h2>
In short: replicates my xbox controller in linux to register multiple virtual controllers.

<h2>Known Issues - for the one redditor who came on over</h2>
![image](https://github.com/LeannAlexandra/virtualController/assets/81232303/7bae4f23-8d29-45a1-8962-17ae4b74c39f)
<ul><li>the eventX has to be set manually using evtest, set to the config file</li>
<li>the eventX gets consumed by the emulator if you run emulator before my app. (this is emulator code, it cannot be freed even when closing the emulator.</li></ul>

