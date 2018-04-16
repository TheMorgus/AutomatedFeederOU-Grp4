#pragma once
#include "LiquidCrystal.h"
#include "Arduino.h"

class Menu {
private:
	void printLoad();
	void printETA();
	void printFeed();

	//Character for a full load bar
	byte load_full[8] = {
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
	};
	//Character for a hollow load bar
	byte load_empty[8] = {
		B11111,
		B10001,
		B10001,
		B10001,
		B10001,
		B10001,
		B10001,
		B11111,
	};

	//This is initialized with dummy pin values because LCD does not have a default constructor, so it must be used
	//with this constructor
	LiquidCrystal lcd = LiquidCrystal(1, 1, 1, 1, 1, 1);

	int load;
	int hours;
	int feedTime;

public:
	void setLoad(int load);
	void setETA(int hours);
	void setFeed(int time);
	void testPrint(long test);
	void testPrint2(long test);

	void update();

	Menu(int RS, int EN, int D4, int D5, int D6, int D7);
};