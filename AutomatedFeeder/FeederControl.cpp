#include "FeederControl.h"
#include "AS5040.h"
#include "Arduino.h"


void FeederController::openDoor() {
	// myServo->write(DOOROPEN_SERVO);
}
void FeederController::closeDoor() {
	// myServo->write(DOORCLOSED_SERVO);
}

void FeederController::motorOn() {
	digitalWrite(motorPin, LOW);
}
void FeederController::motorOff() {
	digitalWrite(motorPin, HIGH);
}

void FeederController::setMotorPin(int motorPin) {

	FeederController::motorPin = motorPin;
}

//void FeederController::dispenseByTime(int time) {
//	long value;
//	int timePassed = 0;
//	//value = myAS5040->encoder_value();
//	//menu->testPrint(value);
//	//menu->testPrint2(value);
//	this->openDoor();
//	delay(1000);
//	this->motorOn();
//	while (timePassed < time) {
//		value = readEncoderPin();
//		menu->dispenseMessage(value, (double)time - (double)timePassed);
//		delay(600);
//		timePassed += 1;
//	}
//	this->motorOff();
//	delay(1000);
//	this->closeDoor();
//}

//int FeederController::dispenseByVolume(int& currentDeg, int& lastDeg, double volume) {
//	int Difference;
//
//}



FeederController::FeederController(Menu* menu, AS5040* encoder) {
	FeederController::menu = menu;
	FeederController::myAS5040 = encoder;
	FeederController::myServo = myServo;
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