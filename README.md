# MOTOR_CONTROL_RASPBERRYPI

Codes for motor control with raspberrypi(C), some simple LED controls and C codings





## Cardgame - oldmaid card game (Simple C coding)

4 players playing oldmaid card game!
Not playing the game in person but automatically




## Golf (Simple LED control)

![image](https://user-images.githubusercontent.com/86711384/151769645-f61daaa5-7d8a-4c02-abcb-20d1dc5a8c03.png)

Playing golf with Raspberry Pi and LEDs
Select your driver(long, short) with the first button
The distance you can make with your driver changes gradually
You can assume the distance with the illuminated LEDs and the brightness of the last LED
Press the button in the exact moment to get the appropriate distance!




## Memory Game (Simple LED control)

![image](https://user-images.githubusercontent.com/86711384/151770320-b795df27-50a9-4ed2-8096-9ada291d78b4.png)

Check your memory and develop it!
LEDs are going to be illuminated in a random order
You have to press the buttons with the same order
The light-up intervals are going to be decreased, so you have to focus hard



## Project 1 - Motor Control : Revolvement

Motor control - motor has to revolve 5 times in clockwise and then same with the other side
Using PID control
Empirically, P: 800, I: 0.9, D: 1 were the optimal Gain values


## Project 2 - Motor Control : Follow the Path

Motor control - motor is going to revolve in decided order
The code reads the order and imitates
Empirically, P: 500, I: 0, D: 10 were the optimal Gain values



## Red Light, Green Light (Simple LED control)

![image](https://user-images.githubusercontent.com/86711384/151772383-2c8e7e03-3a17-4055-a7ec-60fc69e6da4e.png)


Red Light Green Light game is in trend because of Squid Game
Play the game with LEDs and Raspberry Pi
Two buttons are your left and right step which means you have to walk pressing the buttons in the appropriate order
If you press a button successively, you fall down and can't move
Plus, the yellow LED will be on 
You have to touch down for the set time
When green light : press the buttons to walk
When yellow light : you fell down. cannot walk
When red light : you lose the game if you walk in this moment
