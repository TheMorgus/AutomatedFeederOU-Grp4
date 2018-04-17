#pragma once
#include "LiquidCrystal.h"
#include "Arduino.h"
#include "DS3231.h"


enum MenuState { STANDBY, OPTION_TIME, OPTION_FEEDTIME, OPTION_FEEDVOLUME, OPTION_EXIT, OPTION_DEBUG };
enum UserInput { NONE, LEFT, RIGHT, BUTTON };

class Menu {
private:
	void printOptions();
	void printStandby();

	MenuState menuState = STANDBY;

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

	LiquidCrystal lcd;

	bool resetFlag = false;
	int load;
	int hours;
	Time feedTime;

	Time* clockTime;
	Time lastInputTime;

public:
	void setLoad(int load);
	void setFeed(int hr, int min, int sec);
	void flagReset();


	//TESTING FUNCTIONS; DELETE LATER
	void testPrint(long test);
	void testPrint(char* test);
	void testPrint2(long test);
	void print(int column, int row, int test);
	//END TEST FUNCTIONS

	void update();
	void update(UserInput userinput);

	Menu(int RS, int EN, int D4, int D5, int D6, int D7, Time* clockTime);
};

bool operator>(const Time& time1, const Time& time2);
bool operator>(const Time* time1, const Time& time2);
Time operator+(const Time& time1, const int rightsum);
void operator<<(Time& time1, const Time* time2);
