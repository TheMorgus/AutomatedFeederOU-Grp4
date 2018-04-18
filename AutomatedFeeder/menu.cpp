#include "menu.h"
#include "LiquidCrystal.h"
#include "DS3231.h"

//Constructor
//Initializes LCD, creates special characters, gives menu the global clock pointer
Menu::Menu(int EN, int RS, int D4, int D5, int D6, int D7, Time* clockTime): lcd(EN, RS, D4, D5, D6, D7) {
	lcd.begin(20, 4);
	lcd.createChar(0, (uint8_t*)load_full);
	lcd.createChar(1, (uint8_t*)load_empty);
	lcd.createChar(2, (uint8_t*)arrow_up);
	this->clockTime = clockTime;
}


void Menu::resetScreen() {
	lcd.begin(20, 4);
	lcd.createChar(0, (uint8_t*)load_full);
	lcd.createChar(1, (uint8_t*)load_empty);
	lcd.createChar(2, (uint8_t*)arrow_up);
}


void Menu::bubbleSortFeedData() {
	FeedData tempFeedData;
	//sort from highest time to the lowest time
	for (int j = 0; j < 3; j++) {
		for (int i = 0 + j; i < 3; i++) {
			if (feedData[3 - i].time > feedData[3 - i - 1].time) {
				tempFeedData = feedData[3 - i];
				feedData[3 - i] = feedData[3 - i - 1];
				feedData[3 - i - 1] = tempFeedData;
			}
		}
	}
	//sort all non-existant feed times to the end
	for (int j = 0; j < 3; j++) {
		for (int i = 0 + j; i < 3; i++) {
			if (feedData[i].exist == false && feedData[i + 1].exist) {
				tempFeedData = feedData[i + 1];
				feedData[i + 1] = feedData[i];
				feedData[i] = tempFeedData;
			}
		}
	}
}

//Sets the load variable of the class
void Menu::setLoad(int load) {
	if (load > 15) {
		load = 15;
	}
	if (load < 0) {
		load = 0;
	}
	Menu::load = load;
}

//sets variable corresponding to time till next feed
void Menu::setFeed(int hr, int min, int sec) {
	Menu::feedTime.hour = hr;
	Menu::feedTime.min = min;
	Menu::feedTime.sec = sec;
}

void Menu::menuChoiceIncrement() {
	if (timeSetState != OUTSIDE_TIME) {
		tempTime++;
		if (timeSetState == SETHOUR) {
			if (tempTime > 24) {
				tempTime = 0;
			}
		}
		else {
			if (tempTime > 59) {
				tempTime = 0;
			}
		}
	}
	else if (optionState != OUTSIDE ) {
		if (menuState == OPTION_FEEDTIME && optionState != STATE9) {
			switch(optionState) {
			case STATE1:
				if (feedData[0].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState + 2);
				}
				break;
			case STATE3:
				if (feedData[1].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState + 2);
				}
				break;
			case STATE5:
				if (feedData[2].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState + 2);
				}
				break;
			case STATE7:
				if (feedData[3].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState + 2);
				}
				break;
			default:
				optionState = static_cast<OptionState>(optionState + 1);
				break;
			}
		}
		else if (menuState != OPTION_FEEDTIME && optionState != STATE4) {
			optionState = static_cast<OptionState>(optionState + 1);
		}
	}
	else if (optionState == OUTSIDE && menuState != OPTION_DEBUG) {
		menuState = static_cast<MenuState>(menuState + 1);
	}
}

void Menu::menuChoiceDecrement() {
	if (timeSetState != OUTSIDE_TIME) {
		tempTime--;
		if (tempTime <= 0) {
			if (timeSetState == SETHOUR) {
				tempTime = 24;
			}
			else {
				tempTime = 59;
			}
		}
	}
	else if (optionState != OUTSIDE && optionState != STATE1) {
		if (menuState == OPTION_FEEDTIME) {
			switch (optionState) {
			case STATE3:
				if (feedData[0].exist) {
					optionState = static_cast<OptionState>(optionState - 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState - 2);
				}
				break;
			case STATE5:
				if (feedData[1].exist) {
					optionState = static_cast<OptionState>(optionState - 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState - 2);
				}
				break;
			case STATE7:
				if (feedData[2].exist) {
					optionState = static_cast<OptionState>(optionState - 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState - 2);
				}
				break;
			case STATE9:
				if (feedData[3].exist) {
					optionState = static_cast<OptionState>(optionState - 1);
				}
				else {
					optionState = static_cast<OptionState>(optionState - 2);
				}
				break;
			default:
				optionState = static_cast<OptionState>(optionState - 1);
				break;
			}
		}
		if (menuState != OPTION_FEEDTIME) {
			optionState = static_cast<OptionState>(optionState - 1);
		}
	}
	else if (optionState == OUTSIDE && menuState != OPTION_TIME) {
			menuState = static_cast<MenuState>(menuState - 1);
	}
}

void Menu::buttonPush() {

	//if inside the time set portion of the menu
	if (timeSetState != OUTSIDE_TIME) {
		switch (menuState) {
		case(OPTION_TIME):
			if (timeSetState == SETHOUR) {
				rtcClock->setTime(tempTime, clockTime->min, clockTime->sec);
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
				tempTime = 0;
			}
			else if (timeSetState == SETMIN) {
				rtcClock->setTime(clockTime->hour, tempTime, clockTime->sec);
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
				tempTime = 0;
			}
			else if (timeSetState == SETSEC) {
				rtcClock->setTime(clockTime->hour, clockTime->min, tempTime);
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
				tempTime = 0;
			}
			break;
		case(OPTION_FEEDTIME):
			int feedDataPosition;
			//Get the array position of feed data time to modify based on 
			//What choice the user picked on the options
			if (optionState == STATE1) {
				feedDataPosition = 0;
			}
			else if (optionState == STATE3) {
				feedDataPosition = 1;
			}
			else if (optionState == STATE5) {
				feedDataPosition = 2;
			}
			else if (optionState == STATE7) {
				feedDataPosition = 3;
			}
			//Set the time data at the array position and then set the menu state
			//to the next time state to set (Hr->Min->Sec)
			//After seconds, sort the times, exit time adjustment and return to standby menu state
			if (timeSetState == SETHOUR) {
				this->setFeedExist(feedDataPosition, true);
				feedData[feedDataPosition].time.hour = tempTime;
				timeSetState = SETMIN;
				tempTime = 0;
			}
			else if (timeSetState == SETMIN) {
				feedData[feedDataPosition].time.min = tempTime;
				timeSetState = SETSEC;
				tempTime = 0;
			}
			else if (timeSetState == SETSEC) {
				feedData[feedDataPosition].time.sec = tempTime;
				this->bubbleSortFeedData();
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
				tempTime = 0;
			}
			break;
		}
	}
	//if inside a particular option on the menu
	else if (optionState != OUTSIDE) {
		switch (menuState) {
		case (OPTION_TIME):
			if (optionState == STATE1) {
				timeSetState = SETHOUR;
			}
			if (optionState == STATE2) {
				timeSetState = SETMIN;
			}
			if (optionState == STATE3) {
				timeSetState = SETSEC;
			}
			if (optionState == STATE4) {
				optionState = OUTSIDE;
			}
			break;
		case(OPTION_FEEDTIME):
			if (optionState == STATE1) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE2) {
				this->setFeedTime(0, 0, 0, 0);
				this->setFeedExist(0, false);
				this->bubbleSortFeedData();
			}
			else if (optionState == STATE3) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE4) {
				this->setFeedTime(1, 0, 0, 0);
				this->setFeedExist(1, false);
				this->bubbleSortFeedData();
			}
			else if (optionState == STATE5) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE6) {
				this->setFeedTime(2, 0, 0, 0);
				this->setFeedExist(2, false);
				this->bubbleSortFeedData();
			}
			else if (optionState == STATE7) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE8) {
				this->setFeedTime(3, 0, 0, 0);
				this->setFeedExist(3, false);
				this->bubbleSortFeedData();
			}
			else if (optionState == STATE9) {
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
			}
			break;
		}
	}
	//if the menu is displaying the vailable options
	else {
		switch (menuState){
		case OPTION_TIME:
			optionState = STATE1;
			break;
		case OPTION_FEEDTIME:
			optionState = STATE1;
			break;
		case OPTION_FEEDVOLUME:
			break;
		case OPTION_EXIT:
			menuState = STANDBY;
			optionState = OUTSIDE;
			timeSetState = OUTSIDE_TIME;
			break;
		case OPTION_DEBUG:
			break;
		}
	}
	}

void Menu::setFeedTime(int feedPosition, int hour, int min, int sec) {
	feedData[feedPosition].time.hour = hour;
	feedData[feedPosition].time.min = min;
	feedData[feedPosition].time.sec = sec;
}
void Menu::setFeedVolume(int feedPosition, int volume) {
	feedData[feedPosition].volume = volume;
}
void Menu::setFeedExist(int feedPosition, bool existState) {
	feedData[feedPosition].exist = existState;
}

void Menu::passClock(DS3231* rtcClock) {
	Menu::rtcClock = rtcClock;
}

//These functions were made only for testing the motor encoder. !!DELETE WHEN NO LONGER NECESSARY!!
void Menu::testPrint(long test) {
	lcd.setCursor(0, 2);
	lcd.print(test);
}
void Menu::testPrint(char* test) {
	lcd.setCursor(0, 2);
	lcd.print(test);
}
void Menu::testPrint2(long test) {
	lcd.setCursor(6, 2);
	lcd.print(test);
}
void Menu::print(int column, int row, int test) {
	lcd.setCursor(column, row);
	lcd.print(test);
}

//END OF TEST FUNCTIONS; DELETE LATER

void Menu::printOptions() {
	//draw menu
	lcd.setCursor(2, 0);
	lcd.print("SET TIME");
	lcd.setCursor(2, 1);
	lcd.print("SET FEED TIME");
	lcd.setCursor(2, 2);
	lcd.print("SET FEED VOLUME");
	lcd.setCursor(2, 3);
	lcd.print("EXIT");
	lcd.setCursor(13, 3);
	lcd.print("DEBUG");

	//draw cursor
	switch (menuState) {
	case OPTION_TIME:
		lcd.setCursor(0, 0);
		lcd.print("->");
		break;
	case OPTION_FEEDTIME:
		lcd.setCursor(0, 1);
		lcd.print("->");
		break;
	case OPTION_FEEDVOLUME:
		lcd.setCursor(0, 2);
		lcd.print("->");
		break;
	case OPTION_EXIT:
		lcd.setCursor(0, 3);
		lcd.print("->");
		break;
	case OPTION_DEBUG:
		lcd.setCursor(11, 3);
		lcd.print("->");
		break;
	//this should -never- happen
	default:
		lcd.setCursor(13, 0);
		lcd.print("ERROR");
		break;
	}
};

void Menu::printStandby() {
	//Print time line
	lcd.setCursor(5, 0);
	lcd.print("TIME : ");
	lcd.setCursor(12, 0);
	if (clockTime->hour >= 10) {
		lcd.print(clockTime->hour);
	}
	else {
		lcd.setCursor(13, 0);
		lcd.print(clockTime->hour);
	}
	lcd.setCursor(14, 0);
	lcd.print(":");

	lcd.setCursor(15, 0);
	if (clockTime->min >= 10) {
		lcd.print(clockTime->min);
	}
	else {
		lcd.print(0);
		lcd.setCursor(16, 0);
		lcd.print(clockTime->min);
	}
	lcd.setCursor(17, 0);
	lcd.print(":");

	lcd.setCursor(18, 0);
	if (clockTime->sec >= 10) {
		lcd.print(clockTime->sec);
	}
	else {
		lcd.print(0);
		lcd.setCursor(19, 0);
		lcd.print(clockTime->sec);
	}
	//print feed line
	lcd.setCursor(0, 1);
	lcd.print("NEXT FEED@: ");
	lcd.setCursor(12, 1);
	if (feedTime.hour >= 10) {
		lcd.print(feedTime.hour);
	}
	else {
		lcd.setCursor(13, 1);
		lcd.print(feedTime.hour);
	}
	lcd.setCursor(14, 1);
	lcd.print(":");

	lcd.setCursor(15, 1);
	if (feedTime.min >= 10) {
		lcd.print(feedTime.min);
	}
	else {
		lcd.print(0);
		lcd.setCursor(16, 1);
		lcd.print(feedTime.min);
	}
	lcd.setCursor(17, 1);
	lcd.print(":");

	lcd.setCursor(18, 1);
	if (feedTime.sec >= 10) {
		lcd.print(feedTime.sec);
	}
	else {
		lcd.print(0);
		lcd.setCursor(19, 1);
		lcd.print(feedTime.sec);
	}

	//Print Load Line
	//Max of 15 total load bars
	lcd.setCursor(0, 4);
	lcd.print("LOAD:");
	//Fill Full Load Positions
	for (int i = 0; i < load; i++) {
		lcd.setCursor(5 + i, 4);
		lcd.LiquidCrystal::write((uint8_t)0);
	}
	//Fill Empty load Positions
	for (int i = 19; i >= 5 + load; i--) {
		lcd.setCursor(i, 4);
		lcd.LiquidCrystal::write((uint8_t)1);
	}
}


void Menu::printOption_Time() {

	if (timeSetState == OUTSIDE) {
		lcd.setCursor(4, 0);
		lcd.print("ADJUST TIME");
		lcd.setCursor(2, 1);
		if (clockTime->hour >= 10) {
			lcd.print(clockTime->hour);
		}
		else {
			lcd.setCursor(3, 1);
			lcd.print(clockTime->hour);
		}
		lcd.setCursor(6, 1);
		lcd.print(":");

		lcd.setCursor(9, 1);
		if (clockTime->min >= 10) {
			lcd.print(clockTime->min);
		}
		else {
			lcd.print(0);
			lcd.setCursor(10, 1);
			lcd.print(clockTime->min);
		}
		lcd.setCursor(13, 1);
		lcd.print(":");

		lcd.setCursor(16, 1);
		if (clockTime->sec >= 10) {
			lcd.print(clockTime->sec);
		}
		else {
			lcd.print(0);
			lcd.setCursor(17, 1);
			lcd.print(clockTime->sec);
		}
		switch (optionState) {
		case STATE1:
			lcd.setCursor(3, 2);
			lcd.LiquidCrystal::write((uint8_t)2);
			lcd.setCursor(3, 3);
			lcd.print("|");
			break;
		case STATE2:
			lcd.setCursor(10, 2);
			lcd.LiquidCrystal::write((uint8_t)2);
			lcd.setCursor(10, 3);
			lcd.print("|");
			break;
		case STATE3:
			lcd.setCursor(17, 2);
			lcd.LiquidCrystal::write((uint8_t)2);
			lcd.setCursor(17, 3);
			lcd.print("|");
			break;
		case STATE4:
			lcd.setCursor(15, 4);
			lcd.print("EXIT");
			break;
		}
	}
	else {
		lcd.setCursor(4, 0);
		int maxTimeDigit;
		switch (timeSetState) {
		case SETHOUR:
			lcd.print("ADJUST HOUR:");
			maxTimeDigit = 25;
			break;
		case SETMIN:
			lcd.print("ADJUST MIN:");
			maxTimeDigit = 60;
			break;
		case SETSEC:
			lcd.print("ADJUST SEC:");
			maxTimeDigit = 60;
			break;
		}
		for (int i = 0; i < 4; i++){
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempTime) >= maxTimeDigit) {
				lcd.print(i + tempTime - maxTimeDigit);
			}
			else {
				lcd.print(i + tempTime);
			}
		}
		for (int i = 0; i > -4; i--) {
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempTime) < 0) {
				lcd.print(maxTimeDigit + i + tempTime);
			}
			else {
				lcd.print(i + tempTime);
			}
		}

		lcd.setCursor(9, 2);
		lcd.LiquidCrystal::write((uint8_t)2);
		lcd.setCursor(9, 3);
		lcd.print("|");
	}
}

void Menu::printOption_Feedtime() {
	int numFeedTimes= 4; // temp value delete

	if (timeSetState == OUTSIDE_TIME) {
		if (optionState == STATE9) {
			lcd.setCursor(8, 1);
			lcd.print("Exit");
		}
		else {
			for (int i = 0; i < numFeedTimes; i++) {
				if (feedData[i].exist == false) {
					break;
				}
				lcd.setCursor(7, i);
				lcd.print("T :");
				lcd.setCursor(8, i);
				lcd.print(i + 1);
				if (feedData[i].time.hour >= 10) {
					lcd.setCursor(11, i);
				}
				else {
					lcd.setCursor(12, i);
				}
				lcd.print(feedData[i].time.hour);
				lcd.setCursor(13, i);
				lcd.print(":");
				if (feedData[i].time.min >= 10) {
					lcd.setCursor(14, i);
				}
				else {
					lcd.setCursor(15, i);
				}
				lcd.print(feedData[i].time.min);
				lcd.setCursor(16, i);
				lcd.print(":");
				if (feedData[i].time.sec >= 10) {
					lcd.setCursor(17, i);
				}
				else {
					lcd.setCursor(18, i);
				}
				lcd.print(feedData[i].time.sec);

			}
			switch (optionState) {
			case STATE1:
				lcd.setCursor(0, 0);
				lcd.print("->Set");
				break;
			case STATE2:
				lcd.setCursor(0, 0);
				lcd.print("->Rem");
				break;
			case STATE3:
				lcd.setCursor(0, 1);
				lcd.print("->Set");
				break;
			case STATE4:
				lcd.setCursor(0, 1);
				lcd.print("->Rem");
				break;
			case STATE5:
				lcd.setCursor(0, 2);
				lcd.print("->Set");
				break;
			case STATE6:
				lcd.setCursor(0, 2);
				lcd.print("->Rem");
				break;
			case STATE7:
				lcd.setCursor(0, 3);
				lcd.print("->Set");
				break;
			case STATE8:
				lcd.setCursor(0, 3);
				lcd.print("->Rem");
				break;
			}
		}
	}
	else {
		lcd.setCursor(4, 0);
		int maxTimeDigit;
		switch (timeSetState) {
		case SETHOUR:
			lcd.print("ADJUST HOUR:");
			maxTimeDigit = 25;
			break;
		case SETMIN:
			lcd.print("ADJUST MIN:");
			maxTimeDigit = 60;
			break;
		case SETSEC:
			lcd.print("ADJUST SEC:");
			maxTimeDigit = 60;
			break;
		}
		for (int i = 0; i < 4; i++) {
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempTime) >= maxTimeDigit) {
				lcd.print(i + tempTime - maxTimeDigit);
			}
			else {
				lcd.print(i + tempTime);
			}
		}
		for (int i = 0; i > -4; i--) {
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempTime) < 0) {
				lcd.print(maxTimeDigit + i + tempTime);
			}
			else {
				lcd.print(i + tempTime);
			}
		}

		lcd.setCursor(9, 2);
		lcd.LiquidCrystal::write((uint8_t)2);
		lcd.setCursor(9, 3);
		lcd.print("|");
	}
}

void Menu::flagReset() {
	resetFlag = true;
}


void Menu::update(UserInput userInput) {
	//If the user has not used the menu for 10 seconds, return to standby state
	if (userInput == NONE && menuState != STANDBY && clockTime > (lastInputTime + 10)) {
		menuState = STANDBY;
		optionState = OUTSIDE;
		timeSetState = OUTSIDE_TIME;
		tempTime = 0;
		resetFlag = true;
	}
	//Change menu based parameters based on user input
	else if (userInput != NONE) {
		resetFlag = true;
		lastInputTime << clockTime;
		if (menuState == STANDBY) {
			menuState = OPTION_TIME;
		}
		else if (userInput == LEFT) {
			menuChoiceDecrement();
		}
		else if (userInput == RIGHT ) {
			menuChoiceIncrement();
		}
		else if (userInput == BUTTON) {
			buttonPush();
		}
	}
	//Resets the screen when reset flags are up
	if (resetFlag == true) {
		delay(75);
		this->resetScreen();
		resetFlag = false;
	}
	//Prints the scren based on the current state
	if (menuState == STANDBY) {
		printStandby();
	}
	else if (optionState == OUTSIDE) {
		this->printOptions();
	}
	else {
		switch (menuState) {
		case OPTION_TIME:
			this->printOption_Time();
			break;
		case OPTION_FEEDTIME:
			this->printOption_Feedtime();
}
	}
}


bool operator>(const Time& time1, const Time& time2) {
	if (time1.hour > time2.hour) {
		return true;
	}
	else if (time1.hour == time2.hour && time1.min > time2.min) {
		return true;
	}
	else if (time1.hour == time2.hour && time1.min == time2.min && time1.sec > time2.sec) {
		return true;
	}
	else {
		return false;
	}
}
bool operator>(const Time* time1, const Time& time2) {
	if (time1->hour > time2.hour) {
		return true;
	}
	else if (time1->hour == time2.hour && time1->min > time2.min) {
		return true;
	}
	else if (time1->hour == time2.hour && time1->min == time2.min && time1->sec > time2.sec) {
		return true;
	}
	else {
		return false;
	}
}
Time operator+(const Time& time1, const int rightsum) {
	Time returntime;
	returntime.sec = time1.sec + rightsum;
	returntime.min = time1.min;
	returntime.hour = time1.hour;
	bool pass = false;
	while (pass == false) {
		if (returntime.sec >= 60) {
			returntime.min += 1;
			returntime.sec -= 60;
		}
		else {
			pass = true;
		}
	}
	pass = false;
	while (pass == false) {
		if (returntime.min >= 60) {
			returntime.hour += 1;
			returntime.min -= 60;
		}
		else {
			pass = true;
		}
	}
	pass = false;
	while (pass == false) {
		if (returntime.hour >= 24) {
			returntime.hour -= 24;
		}
		else {
			pass = true;
		}
	}
	return returntime;
}

void operator<<(Time& time1, const Time* time2) {
	time1.sec = time2->sec;
	time1.min = time2->min;
	time1.hour = time2->hour;
}
