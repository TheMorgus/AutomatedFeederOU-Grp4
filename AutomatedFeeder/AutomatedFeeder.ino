/*
Name:    AutomatedFeeder.ino
Created: 3/8/2018 8:48:42 PM
Author:  Morg
*/

#include "Wire.h"
#include <DS3231_Simple.h>
//#include <DS3231.h>
//old clock library without I2C function
#include <LiquidCrystal_I2C.h>
#include <Rotary.h>
//#include "LiquidCrystal.h"
#include "menu.h"
#include "AS5040.h"
#include "FeederControl.h"
#include "Servo.h"

//MOTOR ENCODER PINS
const uint8_t ENCODERDATAPIN = 10;
const int ENCODERCLOCKPIN = 9;
const int ENCODERCHIPSELECTPIN = 8;
//RTC PINS
const int DS3231_PIN_SCL = 11;
const int DS3231_PIN_SDL = 10;

//ROTARY PINS
const int ROTARY_PIN_SW = 4;
const int ROTARY_PIN_CLK = 2;
const int ROTARY_PIN_DT = 3;
//SERVO PIN(s)
const int SERVO_PIN = 10;
//MOTOR PIN
const int MOTORPIN = 5;
//ENCODER PIN
const int ENCODERPIN = A0;
//IR SENSOR
const int IRSENSOR = A1;


DateTime time;
FeederSignalPacket* feederSignalPacket;

boolean lastButton = HIGH;
boolean currentButton = HIGH;

Servo myServo;
Rotary rotary(ROTARY_PIN_DT, ROTARY_PIN_CLK);
AS5040* myAS5040;
Menu *menu;
FeederController *feeder;
DS3231_Simple clock;

//Sends directional information from the rotary encoder
//to he menu for processing
void checkUserInput() {
	char result = rotary.process();
	if (result == DIR_CW) {
		menu->flagUpdate(LEFT);
	}
	else if (result == DIR_CCW) {
		menu->flagUpdate(RIGHT);
	}
}

long readEncoder() {
	int _data = 10;
	int _clock = 9;
	int _cs = 8;
	pinMode(_data, INPUT);
	pinMode(_clock, OUTPUT);
	pinMode(_cs, OUTPUT);
	uint32_t raw_value = 0;
	uint16_t inputstream = 0;
	uint16_t c;
	digitalWrite(_cs, HIGH);
	digitalWrite(_clock, HIGH);
	delay(100);
	digitalWrite(_cs, LOW);
	delay(10);
	digitalWrite(_clock, LOW);
	delay(10);
	for (c = 0; c < 16; c++)
	{
		digitalWrite(_clock, HIGH);
		delay(10);
		inputstream = digitalRead(_data);
		raw_value = ((raw_value << 1) + inputstream);

		digitalWrite(_clock, LOW);
		delay(10);
	}
	raw_value = raw_value >> 6;
	raw_value = raw_value * 360 / 1024;
	return(raw_value);
}

void runMotor(int targetDegree) {

}

void setup() {
	myAS5040 = new AS5040(10, 9, 8);
	menu = new Menu(&time);
	myServo.attach(SERVO_PIN);
	feeder = new FeederController(menu, myAS5040);
	clock.begin();
	menu->setLoad(5);
	menu->passClock(&clock);
	//Serial.begin(9600);
	pinMode(10, INPUT);
	pinMode(9, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(IRSENSOR, INPUT);


	pinMode(ROTARY_PIN_SW, INPUT);
	pinMode(MOTORPIN, OUTPUT);
	feeder->setMotorPin(MOTORPIN);
	feeder->setEncoderPin(ENCODERPIN);


	//interrupts for rotary encoder, rotary encoder must be on
	//pins 2 and 3
	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);

	//pointer used for recieving signals in the main loop
	feederSignalPacket = menu->recieveSignalPointer();

	//must get time before loading data
	time = clock.read();
	menu->loadData();
	feeder->motorOff();
}


void loop() {
	myServo.write(150);
	delay(1000);
	myServo.write(30);
	delay(1000);

	time = clock.read();
	//Serial.println("notFried");
	//Get button change information and send to menu
	currentButton = debounce(ROTARY_PIN_SW);


	if (lastButton == HIGH && currentButton == LOW) {

		menu->update(BUTTON);

	}
	else {
		menu->update();
	}
	lastButton = currentButton;
	//This code block checks if the menu is flagging a food dispensement event
	//and runs the motor for the appropriate time in that case
	if (feederSignalPacket->feederSignal != NOSIGNAL) {
		//Do not allow interrupts to program while food is being dispensed
		detachInterrupt(0);
		detachInterrupt(1);
		if (feederSignalPacket->feederSignal == RUN_BYTIME) {
			feeder->dispenseByTime(feederSignalPacket->Val);
		}
		else {
			int lastDeg = readEncoder();
			int currentDeg = readEncoder();
			int totalDeg = 0;
			//this->openDoor();
			//delay(1000);
			feeder->motorOn();
			menu->dispenseMessage(totalDeg, 0, -1);
			while (totalDeg < feederSignalPacket->Val) {
				currentDeg = readEncoder();
				if (lastDeg > 300 && currentDeg < 100) {
					totalDeg += 360 - lastDeg + currentDeg;
					lastDeg = currentDeg;
				}
				else if (currentDeg > lastDeg) {
					totalDeg += currentDeg - lastDeg;
					lastDeg = currentDeg;
				}
				float turns = (float)totalDeg / (float)360;
				if (lastDeg != totalDeg) {
					menu->dispenseMessage(totalDeg, turns, -1);
				}
			}
			feeder->motorOff();
		}
	}


	//motorOff();
	//closeDoor();
	menu->flagReset();
	//clears out menu signal after food is dispensed
	menu->signalRecieved();
	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);
	//delay(250);

}