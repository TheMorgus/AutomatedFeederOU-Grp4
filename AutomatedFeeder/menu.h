#pragma once
#include "LiquidCrystal.h"
#include "Arduino.h"
#include "DS3231.h"


enum MenuState { STANDBY, OPTION_TIME, OPTION_FEEDTIME, OPTION_FEEDVOLUME, OPTION_EXIT, OPTION_DEBUG };
enum OptionState { OUTSIDE, STATE1, STATE2, STATE3, STATE4};
enum TimeSetState { OUTSIDE_TIME, SETHOUR, SETMIN, SETSEC };
enum UserInput { NONE, LEFT, RIGHT, BUTTON };

struct feedTimeArrays {
	Time time;
	int volume;
	bool exist;
};

class Menu {
private:
	void resetScreen();
	void printOptions();
	void printStandby();
	void printOption_Time();
	void printOption_Feedtime();
	void buttonPush();

	MenuState menuState = STANDBY;
	OptionState optionState = OUTSIDE;
	TimeSetState timeSetState = OUTSIDE_TIME;

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

	byte arrow_up[8] = {
		B00100,
		B01110,
		B10101,
		B10101,
		B00100,
		B00100,
		B00100,
		B00100,
	};

	LiquidCrystal lcd;

	bool resetFlag = false;
	int load;
	int hours;
	int tempTime = 0;
	Time feedTime;

	DS3231* rtcClock;
	Time* clockTime;
	Time lastInputTime;

public:
	void setLoad(int load);
	void setFeed(int hr, int min, int sec);
	void flagReset();
	void menuChoiceIncrement();
	void menuChoiceDecrement();
	void passClock(DS3231* rtcClock);


	//TESTING FUNCTIONS; DELETE LATER
	void testPrint(long test);
	void testPrint(char* test);
	void testPrint2(long test);
	void print(int column, int row, int test);
	//END TEST FUNCTIONS

	void update(UserInput userinput = NONE);

	Menu(int RS, int EN, int D4, int D5, int D6, int D7, Time* clockTime);
};

bool operator>(const Time& time1, const Time& time2);
bool operator>(const Time* time1, const Time& time2);
Time operator+(const Time& time1, const int rightsum);
void operator<<(Time& time1, const Time* time2);
