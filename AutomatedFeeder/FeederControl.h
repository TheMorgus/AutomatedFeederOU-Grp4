#pragma once
#include "menu.h"
#include "AS5040.h"
#include "Arduino.h"
//#include "DS3231.h"
#include "Rotary.h"


class FeederController {
private:
	Menu* menu;
	AS5040* myAS5040;
	Servo* myServo;
	int rotaryPosition;




	const int DOOROPEN_SERVO = 160;
	const int DOORCLOSED_SERVO = 70;

	int motorPin;
	int encoderPin;

public:
	void openDoor();
	void closeDoor();
	void setMotorPin(int motorPin);
	void setEncoderPin(int encoderPin);
	long readEncoderPin();
	//This is a secondary dispensment function that is only coded to be run when a debug motor run by time
	//even is selected by the user through the menu. This dispensement is time based and will display the time
	//remaining till the motor stops on the screen.
	void dispenseByTime(int time);
	//This is the main dispensement function that is ran when the menu detects that it is time for a user specified 
	//feed event. 
	int dispenseByVolume(int& currentDeg, int& lastDeg, double volume);
	//void checkUserInput();
	void motorOn();
	void motorOff();
	FeederController(Menu* menu, AS5040* encoder);
};

//basic debouncing function to ensure signal legitimacy and minimize double button press events
boolean debounce(int button);
