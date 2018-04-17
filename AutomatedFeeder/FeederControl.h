#pragma once
#include "menu.h"
#include "AS5040.h"
#include "Arduino.h"
#include "DS3231.h"
#include "Rotary.h"


class FeederController {
private:
	Menu* menu;
	AS5040* myAS5040;
	int rotaryPosition;

public:
	void dispenseFood();
	//void checkUserInput();

	FeederController(Menu* menu, AS5040* encoder);
};

boolean debounce(int button);