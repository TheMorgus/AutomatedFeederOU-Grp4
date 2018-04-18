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
	else if (optionState == OUTSIDE && menuState != OPTION_DEBUG) {
		menuState = static_cast<MenuState>(menuState + 1);
	}
	else if (optionState != OUTSIDE && optionState != STATE4)
		optionState = static_cast<OptionState>(optionState + 1);

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
	else if (optionState == OUTSIDE && menuState != OPTION_TIME) {
		menuState = static_cast<MenuState>(menuState - 1);
	}
	else if (optionState != OUTSIDE && optionState != STATE1)
		optionState = static_cast<OptionState>(optionState - 1);

}

void Menu::buttonPush() {

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
			if (timeSetState == SETHOUR) {
				timeSetState = SETMIN;
				tempTime = 0;
			}
			else if (timeSetState == SETMIN) {
				timeSetState = SETSEC;
				tempTime = 0;
			}
			else if (timeSetState == SETSEC) {
				menuState = STANDBY;
				optionState = OUTSIDE;
				timeSetState = OUTSIDE_TIME;
				tempTime = 0;
			}
			break;
		}
	}
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
			if (optionState == STATE2) {
				timeSetState = SETHOUR;
			}
			if (optionState == STATE3) {
				timeSetState = SETHOUR;
			}
			if (optionState == STATE4) {
				timeSetState = SETHOUR;
			}
			break;
		}
	}
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
		for (int i = 0; i < numFeedTimes; i++) {
			lcd.setCursor(3, i);
			lcd.print("F.Time :");
			lcd.setCursor(9, i);
			lcd.print(i + 1);
		}
		switch (optionState) {
		case STATE1:
			lcd.setCursor(0, 0);
			lcd.print("->");
			break;
		case STATE2:
			lcd.setCursor(0, 1);
			lcd.print("->");
			break;
		case STATE3:
			lcd.setCursor(0, 2);
			lcd.print("->");
			break;
		case STATE4:
			lcd.setCursor(0, 3);
			lcd.print("->");
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
