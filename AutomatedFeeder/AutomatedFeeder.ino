/*
 Name:		AutomatedFeeder.ino
 Created:	3/8/2018 8:48:42 PM
 Author:	Morg
*/

#include "LiquidCrystal.h"
#include "menu.h"
#include "AS5040.h"
#include "FeederControl.h"

const uint8_t ENCODERDATAPIN = A0;
const int ENCODERCLOCKPIN = 9;
const int ENCODERCHIPSELECTPIN = 8;
const int LCDRS_PIN = 2;
const int LCDEN_PIN = 3;
const int LCDD4_PIN = 4;
const int LCDD5_PIN = 5;
const int LCDD6_PIN = 6;
const int LCDD7_PIN = 7;

Menu menu(LCDRS_PIN, LCDEN_PIN, LCDD4_PIN, LCDD5_PIN, LCDD6_PIN, LCDD7_PIN);
FeederController Feeder(&menu, &myAS5040);

AS5040 myAS5040(ENCODERDATAPIN, ENCODERCLOCKPIN, ENCODERCHIPSELECTPIN);



void setup() {
	menu.setLoad(5);
	menu.setETA(30);
	menu.setFeed(1030);
}


void loop() {
	menu.update();
	Feeder.dispenseFood();
}