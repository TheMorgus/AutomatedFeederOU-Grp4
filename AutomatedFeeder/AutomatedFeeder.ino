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
}

void loop() {
	menu.setLoad(8);
	menu.update();
}