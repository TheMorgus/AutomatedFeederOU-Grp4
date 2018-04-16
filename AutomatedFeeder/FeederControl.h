#pragma once
#include "menu.h"
#include "AS5040.h"
#include "Arduino.h"

class FeederController {
private:
	Menu* menu;
	AS5040* myAS5040;

public:
	void dispenseFood();

	FeederController(Menu* menu, AS5040* encoder);
};