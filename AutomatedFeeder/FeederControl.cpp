#include "FeederControl.h"
#include "AS5040.h"
#include "Arduino.h"

void FeederController::dispenseFood() {

	long value;
	value = myAS5040->encoder_value();
	menu->testPrint(value);
	value = myAS5040->encoder_degrees();
	menu->testPrint2(value);
}


FeederController::FeederController(Menu* menu, AS5040* encoder){
	FeederController::menu = menu;
	FeederController::myAS5040 = encoder;
	rotaryPosition = 0;
}


