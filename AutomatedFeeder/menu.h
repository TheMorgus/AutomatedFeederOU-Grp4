#pragma once
#include "LiquidCrystal.h"
#include "Arduino.h"
#include "DS3231.h"
#include "EEPROM.h"


enum MenuState { STANDBY, OPTION_TIME, OPTION_FEEDTIME, OPTION_FEEDVOLUME, OPTION_EXIT, OPTION_DEBUG };
enum OptionState { OUTSIDE, STATE1, STATE2, STATE3, STATE4, STATE5, STATE6, STATE7, STATE8, STATE9};
enum TimeSetState { OUTSIDE_TIME, SETHOUR, SETMIN, SETSEC };
enum VolumeSetState { OUTSIDE_VOLUME, SETWHOLEDIGIT, SETPARTIALDIGIT};
enum DebugSetState {OUTSIDE_DEBUG, DEBUG_MOTORTIME, DEBUG_MOTORVOLUME, DEBUG_ENCODERPOSITION, DEBUG_IRSENSOR};
enum UserInput { NONE, LEFT, RIGHT, BUTTON };
enum FeederSignal { NOSIGNAL, RUN_BYTIME, RUN_BYVOLUME };

struct FeedData{
	Time time;
	double volume;
	bool exist;
};

struct FeederSignalPacket {
	FeederSignal feederSignal;
	double Val;
};

class Menu {
private:
	//reinitializes the LCD and recreates the custom characters
	//this is needed because the interrupts used for the rotary encoder have
	//a tendency to cause lcd errors
	void resetScreen();
	//sets the class variables to default states, so that when the menu class runs through
	//its update loop, it will display the standby screen
	void returnToStandby();
	//Increments the menu state based on which portion of the lcd screen the user is in
	void menuChoiceIncrement();
	//Decrements the menu state based on which portion of the lcd screen the user is in
	void menuChoiceDecrement();
	//Raises/lowers flag, and sets variables based on what the user has selected and what
	//state the menu is in
	void buttonPush();
	//prints the standby screen of the menu which shows the user basic information on the state
	//of the feeder
	void printStandby();
	//prints the available options to the user
	void printOptions();
	//prints the time-setting portion of the menu to the user, as well as an hour/min/sec setting portion
	//when the user makes selections
	void printOption_Time();
	//prints the feed-time setting part of the menu, as well as the hour/min/sec adjustment if the user chooses
	//to make adjustments
	void printOption_Feedtime();
	//prints the part of the menu concerned with altering the feed volume. When a feed-volume is selected for adjustment
	//it will display the adjustments available to the user
	void printOption_FeedVolume();
	//prints the available debug options available for the user, as well as the respective debug option screens when they are selected
	void printOption_PrintDebug();
	//sets the feed-time in the feed data array based on its feed index value supplied
	void setFeedTime(int feedPosition, int hour, int min, int sec);
	//sets the volume in the feed data array based on the feed index value supplied
	void setFeedVolume(int feedPosition, double volume);
	//sets a flag at the feed index supplied that signifies is a feed-time exist at that location
	void setFeedExist(int feedPosition, bool existState);
	//sorts the feedData using a bubble sort where the highest time is in the lowest index position and
	//the higher index position have lower times.
	//feed times that do not exist are located together at the end of the feedtime array
	void bubbleSortFeedData();
	//finds the position of the next feed time relative to the current clock time
	//it then sets the nextFedPos member of the class to the index value of that feed time
	void findNextFeed();
	//Saves the feedData array that includes: feed-time, feed-volume, and a feed existence boolean to the
	//EEPROM located on the Arduino device.
	//WARNING: The arduino EEPROM has a maximum limit on the write-cycles of the EEPROM,
	//so minimize the changes to any feed data, as the eeprom is rewritten on all feedData changes
	void saveData();

	//These next 5 booleans and the states they represent serve as signals to the menu on the following:
	//1)What to display on the lcd screen
	//2)How turning the rotary encoding (incrementing/decrement) effects the menu state
	//3)What a button push signifies and how to respond to it
	MenuState menuState = STANDBY;
	OptionState optionState = OUTSIDE;
	TimeSetState timeSetState = OUTSIDE_TIME;
	VolumeSetState volumeSetState = OUTSIDE_VOLUME;
	DebugSetState debugSetState = OUTSIDE_DEBUG;

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
	//Character for an up arrow. This arrow is used to represent the choice the user would be selecting
	//with a button push on various parts of the menu
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
	int tempValue = 0;
	int nextFeedPos;
	//All the feedtime data is located in this struct
	FeedData feedData[4];
	FeederSignalPacket feederSignalPacket;
	DS3231* rtcClock;
	Time* clockTime;
	Time lastInputTime;

public:
	//For testing shouldn't be used during normal operations
	void setLoad(int load);
	void testPrint(long test);
	void testPrint(char* test);
	void testPrint2(long test);
	void print(int column, int row, int test);
	//END OF TESTING

	//Flags a screen reset for the next run of the menus update loop
	void flagReset();
	//Loads data from the EEPROM
	//Should be run in the setup of the main function
	void loadData();
	void passClock(DS3231* rtcClock);
	FeederSignalPacket* recieveSignalPointer();
	//Indicates to the menu that the signal indicating a motor run event was recieved
	//In this case, the signal values are reset to the off state
	void signalRecieved();
	//Displays a message on the lcd screen for motor run events
	void dispenseMessage(long encoderDegree, int timeRemaining = -1);

	//The main loop of the menu program. Everytime the program runs through this loop, it decideds
	//what to display based on the internal variable flags that were raised due to user input
	void update(UserInput userinput = NONE);
	
	//Constructor
	//Initializes LCD, creates special characters, gives menu the global clock pointer
	Menu(int RS, int EN, int D4, int D5, int D6, int D7, Time* clockTime);
};

//Allows a comparison on wether the time contained in a time struct
//is at a later period than another time struct
bool operator>(const Time& time1, const Time& time2);
bool operator>(const Time* time1, const Time& time2);
//when an integer is added to a time struct, that integer is added as
//if it a value in seconds
Time operator+(const Time& time1, const int rightsum);
//assigns the time struc on the right hand side of the stream operator
//to the time struc on the left hand side
void operator<<(Time& time1, const Time* time2);
