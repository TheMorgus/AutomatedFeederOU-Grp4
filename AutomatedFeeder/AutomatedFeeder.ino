/*
 Name:		AutomatedFeeder.ino
 Created:	3/8/2018 8:48:42 PM
 Author:	Morg
*/

#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
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
const int LCDRS_PIN = 12;
const int LCDEN_PIN = 13;
const int LCDD4_PIN = 4;
const int LCDD5_PIN = 5;
const int LCDD6_PIN = 6;
const int LCDD7_PIN = 7;
//ROTARY PINS
const int ROTARY_PIN_SW = 0;
const int ROTARY_PIN_CLK = 2;
const int ROTARY_PIN_DT = 3;

int rotaryposition = 0;
Time time;

boolean lastButton = HIGH;
boolean currentButton = HIGH;


Rotary rotary(ROTARY_PIN_DT, ROTARY_PIN_CLK);
AS5040 myAS5040(ENCODERDATAPIN, ENCODERCLOCKPIN, ENCODERCHIPSELECTPIN);
Menu menu(LCDRS_PIN, LCDEN_PIN, LCDD4_PIN, LCDD5_PIN, LCDD6_PIN, LCDD7_PIN, &time);
FeederController feeder(&menu, &myAS5040);
DS3231 clock(DS3231_PIN_SDL, DS3231_PIN_SCL);

//Interupt on rotary encoder movement.
void checkUserInput() {
	char result = rotary.process();
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
	//For testing clock, delete when functionality added through menu
	//clock.setTime(2, 13, 15);
	//clock.setDate(1, 2, 1920);
	menu.setLoad(5);
	menu.setFeed(10, 25, 0);
	menu.passClock(&clock);

	pinMode( ROTARY_PIN_SW , INPUT);

	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);

	//test delete
	//Serial.begin(9600);
}


void loop() {
	time = clock.getTime();
	currentButton = debounce(ROTARY_PIN_SW);
	//testdelete
	//Serial.print(digitalRead(ROTARY_PIN_SW));
	if (lastButton == HIGH && currentButton == LOW) {
		menu.update(BUTTON);
	}
	else {
		menu.update();
	}
	lastButton = currentButton;
	delay(50);
	//Feeder.dispenseFood();
	//Testing Shit DELETE LATER
	//int hour = time.hour;
	//int min = time.min;
	//int sec = time.sec;
	//menu.print(0, 2, hour);
	//menu.print(3, 2, min);
	//menu.print(6, 2, sec);
	//END OF TEST; DELETE LATER
}