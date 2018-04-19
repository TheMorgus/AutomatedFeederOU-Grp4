/*
 Name:		AutomatedFeeder.ino
 Created:	3/8/2018 8:48:42 PM
 Author:	Morg
*/

#include <DS3231_Simple.h>
//#include <DS3231.h>
//old clock library without I2C function
//#include <LiquidCrystal_I2C.h>
#include <Rotary.h>
#include "LiquidCrystal.h"
#include "menu.h"
#include "AS5040.h"
#include "FeederControl.h"

//MOTOR ENCODER PINS
const uint8_t ENCODERDATAPIN = A0;
const int ENCODERCLOCKPIN = 9;
const int ENCODERCHIPSELECTPIN = 8;
//RTC PINS
const int DS3231_PIN_SCL = 11;
const int DS3231_PIN_SDL = 10;
//LCD PINS
const int LCDRS_PIN = 4;
const int LCDEN_PIN = 5;
const int LCDD4_PIN = 6;
const int LCDD5_PIN = 7;
const int LCDD6_PIN = 8;
const int LCDD7_PIN = 9;
//ROTARY PINS
const int ROTARY_PIN_SW = 0;
const int ROTARY_PIN_CLK = 2;
const int ROTARY_PIN_DT = 3;


DateTime time;
FeederSignalPacket* feederSignalPacket;

boolean lastButton = HIGH;
boolean currentButton = HIGH;


Rotary rotary(ROTARY_PIN_DT, ROTARY_PIN_CLK);
AS5040 myAS5040(ENCODERDATAPIN, ENCODERCLOCKPIN, ENCODERCHIPSELECTPIN);
Menu menu(LCDRS_PIN, LCDEN_PIN, LCDD4_PIN, LCDD5_PIN, LCDD6_PIN, LCDD7_PIN, &time);
FeederController feeder(&menu, &myAS5040);
DS3231_Simple clock;

//Sends directional information from the rotary encoder
//to he menu for processing
void checkUserInput() {
	char result = rotary.process();
	menu.clearScreen();
	menu.flagReset();
	if (result == DIR_CW) {
		menu.update(LEFT);
	}
	else if (result == DIR_CCW){
		menu.update(RIGHT);
	}
}

void setup() {
	clock.begin();
	menu.setLoad(5);
	menu.passClock(&clock);

	pinMode( ROTARY_PIN_SW , INPUT);

	//interrupts for rotary encoder, rotary encoder must be on
	//pins 2 and 3
	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);

	//pointer used for recieving signals in the main loop
	feederSignalPacket = menu.recieveSignalPointer();

	//must get time before loading data
	time = clock.read();
	menu.loadData();
}


void loop() {
	time = clock.read();
	//Get button change information and send to menu
	currentButton = debounce(ROTARY_PIN_SW);
	if (lastButton == HIGH && currentButton == LOW) {
		menu.update(BUTTON);
	}
	else {
		menu.update();
	}
	lastButton = currentButton;
	//This code block checks if the menu is flagging a food dispensement event
	//and runs the motor for the appropriate time in that case
	if (feederSignalPacket->feederSignal != NONE) {
		//Do not allow interrupts to program while food is being dispensed
		detachInterrupt(0);
		detachInterrupt(1);
		if (feederSignalPacket->feederSignal == RUN_BYTIME) {
			feeder.dispenseByTime(feederSignalPacket->Val);
		}
		else {
			feeder.dispenseByVolume(feederSignalPacket->Val);
		}
		menu.flagReset();
		//clears out menu signal after food is dispensed
		menu.signalRecieved();
		attachInterrupt(0, checkUserInput, CHANGE);
		attachInterrupt(1, checkUserInput, CHANGE);
	}
	delay(100);
}