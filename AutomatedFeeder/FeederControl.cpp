#include "FeederControl.h"
#include "AS5040.h"
#include "Arduino.h"

void FeederController::dispenseByTime(int time) {
	long value;
	int timePassed = 0;
	//value = myAS5040->encoder_value();
	//menu->testPrint(value);
	//menu->testPrint2(value);
	while (timePassed < time) {
		value = myAS5040->encoder_degrees();
		menu->dispenseMessage(value, (double)time - (double)timePassed);
		delay(600);
		timePassed += 1;
	}
}

void FeederController::dispenseByVolume(double volume) {
	long value;
	//value = myAS5040->encoder_value();
	//menu->testPrint(value);
	value = myAS5040->encoder_degrees();
	//menu->testPrint2(value);
	menu->dispenseMessage(value);
	delay(5000);
}


FeederController::FeederController(Menu* menu, AS5040* encoder){
	FeederController::menu = menu;
	FeederController::myAS5040 = encoder;
	rotaryPosition = 0;
}



boolean debounce(int buttonpin) {
	boolean state1;
	boolean state2;
	boolean state3;
	do {
		state1 = digitalRead(buttonpin);
		delay(5);
		state2 = digitalRead(buttonpin);
		delay(5);
		state3 = digitalRead(buttonpin);
		delay(5);
	} while (state1 != state2 || state1 != state3);
	return state1;
}