/*
Name:    AutomatedFeeder.ino
Created: 3/8/2018 8:48:42 PM
Author:  Morg
*/

#include "Wire.h"
#include <DS3231_Simple.h>
#include <LiquidCrystal_I2C.h>
#include <Rotary.h>
#include "menu.h"
#include "AS5040.h"
#include "Servo.h"

//MOTOR ENCODER PINS
const int ENCODERDATAPIN = 10;
const int ENCODERCLOCKPIN = 9;
const int ENCODERCHIPSELECTPIN = 8;
//ROTARY PINS
const int ROTARY_PIN_SW = 4;
const int ROTARY_PIN_CLK = 2;
const int ROTARY_PIN_DT = 3;
//SERVO PIN(s)
const int SERVO_PIN = 11;
//Values to send to IR sensor at the respective states
const int SERVO_DOOROPEN = 20;
const int SERVO_DOORCLOSED = 180;
//MOTOR PIN
const int MOTORPIN = 5;
//IR SENSOR
const int IRSENSOR = A1;

DateTime time;
FeederSignalPacket* feederSignalPacket;

Servo myServo;
Rotary rotary(ROTARY_PIN_DT, ROTARY_PIN_CLK);
AS5040* myAS5040;
Menu *menu;
DS3231_Simple clock;

//for debouncing the button
boolean lastButton = HIGH;
boolean currentButton = HIGH;

//rotary encoder movement triggers interrupts, those interrupts
//call this function, which tells the menu which direction the user is turning
//the encoder
void checkUserInput() {
	char result = rotary.process();
	if (result == DIR_CW) {
		menu->flagUpdate(LEFT);
	}
	else if (result == DIR_CCW) {
		menu->flagUpdate(RIGHT);
	}
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

void runMotorTime(int targetTime) {
	int currentDeg = readEncoder();
	int lastDeg = currentDeg;
	int totalDeg = 0;
	int time = 0;
	//open feeder doors before dispensing food
	myServo.write(SERVO_DOOROPEN);
	delay(1000);
	motorOn();
	menu->dispenseMessage(totalDeg, 0, targetTime);
	//turn the motor until the elapsed time has passed
	while (time < targetTime) {
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
		menu->dispenseMessage(totalDeg, turns, targetTime - time);
		time++;
		//delay of 560 is used to approximate one second through one iteration of the loop
		//this is because the encoder read has a combined delay of 440 coded into the function
		delay(560);
	}
	//close doors after dispensing food
	motorOff();
	delay(1000);
	myServo.write(SERVO_DOORCLOSED);
}

void runMotorVolume(int targetDegree) {
	int lastDeg = readEncoder();
	int currentDeg = readEncoder();
	int totalDeg = 0;
	myServo.write(SERVO_DOOROPEN);
	//open feeder doors before dispensing food
	delay(1000);
	motorOn();
	menu->dispenseMessage(totalDeg, 0, -1);
	//turn the motor until the auger has rotated by the target
	//degree value
	while (totalDeg < targetDegree) {
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
	motorOff();
	//close doors after dispensing food
	delay(1000);
	myServo.write(SERVO_DOORCLOSED);
}

void motorOn() {
	digitalWrite(MOTORPIN, LOW);
}

void motorOff() {
	digitalWrite(MOTORPIN, HIGH);
}

void setup() {
	myAS5040 = new AS5040(10, 9, 8);
	menu = new Menu(&time);

	myServo.attach(SERVO_PIN);

	//Serial.begin(9600);
	//Encoder pins
	pinMode(ENCODERDATAPIN, INPUT);
	pinMode(ENCODERCLOCKPIN, OUTPUT);
	pinMode(ENCODERCHIPSELECTPIN, OUTPUT);
	//IR pins
	pinMode(IRSENSOR, INPUT);
	//Rotary button pin
	pinMode(ROTARY_PIN_SW, INPUT);
	//Motor pin
	pinMode(MOTORPIN, OUTPUT);

	//interrupts for rotary encoder, rotary encoder must be on
	//pins 2 and 3
	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);

	//pointer used for recieving signals in the main loop
	feederSignalPacket = menu->recieveSignalPointer();

	//must get time before loading data
	clock.begin();
	menu->setLoad(5);
	menu->passClock(&clock);
	time = clock.read();
	menu->loadData();
	motorOff;
}

void loop() {
	time = clock.read();
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
			runMotorTime(feederSignalPacket->Val);
		}
		else {
			runMotorVolume(feederSignalPacket->Val);
		}
		menu->flagReset();
		menu->signalRecieved();
		//reattach interrupts after motor run event;
		attachInterrupt(0, checkUserInput, CHANGE);
		attachInterrupt(1, checkUserInput, CHANGE);
	}
}