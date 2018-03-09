/*
 Name:		AutomatedFeeder.ino
 Created:	3/8/2018 8:48:42 PM
 Author:	Morg
*/

#include "LiquidCrystal.h"
#include "menu.h"

int time = 0;

Menu menu;

void setup() {
	menu = Menu();
	menu.setLoad(5);
	menu.setETA(30);
	menu.setFeed(1030);
}

void loop() {

	menu.update();
}