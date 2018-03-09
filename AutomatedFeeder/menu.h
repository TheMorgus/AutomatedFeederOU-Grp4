#pragma once
#include "LiquidCrystal.h"
#include "Arduino.h"

class Menu {
private:
	void printLoad();
	void printETA();
	void printFeed();
public:
	LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);

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

	int load;
	int hours;
	int feedTime;

	
	
	void setLoad(int load);
	void setETA(int hours);
	void setFeed(int time);

	void update();

	Menu();
};