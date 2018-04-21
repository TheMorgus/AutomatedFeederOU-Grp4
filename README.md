# AutomatedFeederOU-Grp4

Description: Code for an arduino microcontroller that automates an electromechnical dogfeeder by controlling:

1)An 20x4 LCD screen and rotary encoder for user input

2)An I2C backpack for the LCD screen

3)A 12V DC motor that powers an auger that dispenses feed out of the device

4)A servo motor that is connected to a door on the front of the box(that covers the auger hole) by a pushrod. 

5)An IR sensor to read the height of dogfood in the feeders hopper. This measurement is used to approximate the remaining food
in the device

6)A AS5040 analog encoder to determine the total rotation of the auger. This rotation is used by the microcontroller so that the user can set
precise feed volumes through the menu.

7)A DS3231 RTC clock for precise time keeping. This allows the user to set feed times for dog food to be automatically dispensed



Libraries Used

1)AS5040
http://madscientisthut.com/forum_php/viewtopic.php?f=11&t=17
by "John"

Used For: Convert the encoder signals into positional values

2)DS3231_Simple
https://github.com/sleemanj/DS3231_Simple
by sleemanj(github)
Used For: Connect to the DS3231 using I2C communication

3)LiquidCrystal_I2C
https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
Author: Frank de Brabander, Maintainer: Marco Schwartz
Used For: Connect and interface with 20x4 LCD using I2C communication


4)Rotary
https://github.com/brianlow/Rotary
Author: Ben Buxton
Used For: Accurate and easy rotary encoder processing

Installation

1)Place the libraries in the arduino's libraries folder
then either

2.a)BUILD IN VISUAL STUDIO
  i)Open the solution
  
  ii)Using the ARDUINO IDE for Visual Studio Extension, add the libraries used (The ones located in the libraries used folder) using
  the add libraries button on the top toolbar
  
  iii)Ensure Arduino is connected and the right com-port is selected in the top tool bar
  iv)build 
  
  
2.b)BUILD IN ARDUINO IDE

  i)Either copy paste the AutomatedFeeder.ino text into a sketch using
    a)The arduino to open the solution
    b)Github to open the file as text
    
  ii)Copy and paste the header files and cpp files for "AS5040" and "menu" into your
  sketch folder
  
  iii)compile->send to arduino
