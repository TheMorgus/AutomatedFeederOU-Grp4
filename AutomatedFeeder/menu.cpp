#include "menu.h"

Menu::Menu(DateTime* clockTime) : lcd(0x27, 20, 4) {
	lcd.init();
	lcd.backlight();
	lcd.begin(20, 4);
	lcd.createChar(0, (uint8_t*)load_full);
	lcd.createChar(1, (uint8_t*)load_empty);
	lcd.createChar(2, (uint8_t*)arrow_up);

	this->clockTime = clockTime;

	feederSignalPacket.feederSignal = NOSIGNAL;
	feederSignalPacket.Val = 0;
}
void Menu::clearScreen() {
	lcd.clear();
}
void Menu::returnToStandby() {
	menuState = STANDBY;
	optionState = OUTSIDE;
	timeSetState = OUTSIDE_TIME;
	volumeSetState = OUTSIDE_VOLUME;
	debugSetState = OUTSIDE_DEBUG;
	tempValue = 0;
	//menu is flagged for reset on any returns to standby to ensure that the lcd screen
	//is running accurately
	flagReset();
}
void Menu::findNextFeed() {
	//nextFeedPos of -2 indicates that there are no existant feed times.
	//nextFeedPos of -1 indicates there are no more feed times today
	for (int i = 0; i < 4; i++) {
		if (feedData[0].exist == false) {
			nextFeedPos = -2;
			break;
		}
		if (feedData[3 - i].exist == false) {
			continue;
		}
		else {
			if (clockTime > feedData[3 - i].time) {
				//if the current time is greater than all the available feed times
				//there are no more feedings today
				if (3 - i == 0) {
					nextFeedPos = -1;
				}
				continue;
			}
			else {
				nextFeedPos = 3 - i;
				break;
			}
		}
	}
}
void Menu::saveData() {
	int address = 0;
	for (int i = 0; i < 4; i++) {
		EEPROM.put(address, feedData[i]);
		address += 13;
	}
	EEPROM.put(address, feedData[1]);
}
void Menu::loadData() {
	int address = 0;
	for (int i = 0; i < 4; i++) {
		EEPROM.get(address, feedData[i]);
		address += 13;
	}
	//bubble sort feed data on load not only to ensure its in a legitimate position,
	//but because the bubble sort function signals to the menu to determine the next legitimate feed time
	//to dispense food with the include findNextFeed() functions
	this->bubbleSortFeedData();
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
	//A bubble sort is necessary for the findNextFeed function to work properly
	this->findNextFeed();
}
void Menu::setLoad(int load) {
	if (load > 15) {
		load = 15;
	}
	if (load < 0) {
		load = 0;
	}
	Menu::load = load;
}
void Menu::menuChoiceIncrement() {
	//If user is setting the time in either the feedtime selection, or current time selection
	if (timeSetState != OUTSIDE_TIME) {
		tempValue++;
		if (timeSetState == SETHOUR) {
			if (tempValue > 24) {
				tempValue = 0;
			}
		}
		else {
			if (tempValue > 59) {
				tempValue = 0;
			}
		}
	}
	else if (volumeSetState != OUTSIDE_VOLUME) {
		tempValue++;
		if (volumeSetState == SETWHOLEDIGIT) {
			if (tempValue > 9) {
				tempValue = 0;
			}
		}
		else {
			if (tempValue > 99) {
				tempValue = 0;
			}
		}
	}
	//if the user is in a debug setting
	else if (debugSetState != OUTSIDE_DEBUG) {
		switch (debugSetState) {
		case DEBUG_MOTORTIME:
			tempValue++;
			if (tempValue > 20) {
				tempValue = 0;
			}
			break;
		case DEBUG_MOTORVOLUME:
			tempValue++;
			break;
		}
	}
	//if the user is in any of the individual option menus, but not any of the secondary
	//deeper levels of those menus
	else if (optionState != OUTSIDE) {
		if (menuState == OPTION_TIME && optionState != STATE4) {
			optionState = static_cast<OptionState>(optionState + 1);
		}
		else if (menuState == OPTION_FEEDTIME && optionState != STATE9) {
			switch (optionState) {
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
		else if (menuState == OPTION_FEEDVOLUME && optionState != STATE5) {
			switch (optionState) {
			case STATE1:
				if (feedData[1].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = STATE5;
				}
				break;
			case STATE2:
				if (feedData[2].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = STATE5;
				}
				break;
			case STATE3:
				if (feedData[3].exist) {
					optionState = static_cast<OptionState>(optionState + 1);
				}
				else {
					optionState = STATE5;
				}
				break;
			default:
				optionState = static_cast<OptionState>(optionState + 1);
				break;

			}
		}
		else if (menuState == OPTION_DEBUG && optionState != STATE3) {
			optionState = static_cast<OptionState>(optionState + 1);
		}
	}
	//If the user is on the option select screen
	else if (optionState == OUTSIDE && menuState != OPTION_DEBUG) {
		menuState = static_cast<MenuState>(menuState + 1);
	}
}
void Menu::menuChoiceDecrement() {
	//if user is on the feed-time or current time selection menu
	if (timeSetState != OUTSIDE_TIME) {
		tempValue--;
		if (tempValue < 0) {
			if (timeSetState == SETHOUR) {
				//max hour value is 24
				tempValue = 24;
			}
			else {
				//max min/sec value = 60
				tempValue = 59;
			}
		}
	}
	//if user is on the feed-volume selection menu
	else if (volumeSetState != OUTSIDE_VOLUME) {
		tempValue--;
		if (tempValue < 0) {
			if (volumeSetState == SETWHOLEDIGIT) {
				//max of 10 cups allowed to be dispensed
				tempValue = 10;
			}
			else {
				tempValue = 99;
			}
		}
	}
	//if in the second level of the debug portion of the menu
	else if (debugSetState != OUTSIDE_DEBUG) {
		switch (debugSetState) {
		case DEBUG_MOTORTIME:
			tempValue--;
			if (tempValue < 0) {
				tempValue = 20;
			}
			break;
		case DEBUG_MOTORVOLUME:
			tempValue--;
			if (tempValue <= -10) {
				tempValue = -10;
			}
			break;
		}
	}
	//If user in this first level of any of the available options
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
		else if (menuState == OPTION_TIME) {
			optionState = static_cast<OptionState>(optionState - 1);
		}
		else if (menuState == OPTION_FEEDVOLUME) {
			switch (optionState) {
			case STATE5:
				for (int i = 0; i < 4; i++) {
					if (feedData[3 - i].exist) {
						optionState = static_cast<OptionState>(4 - i);
						break;
					}
				}
				break;
			default:
				optionState = static_cast<OptionState>(optionState - 1);
				break;
			}
		}
		else if (menuState == OPTION_DEBUG) {
			optionState = static_cast<OptionState>(optionState - 1);
		}
	}
	//if user is in the option select screen
	else if (optionState == OUTSIDE && menuState != OPTION_TIME) {
		menuState = static_cast<MenuState>(menuState - 1);
	}
}
void Menu::buttonPush() {

	//if inside the time set portion of the menu
	if (timeSetState != OUTSIDE_TIME) {
		DateTime tempTime;
		switch (menuState) {
		case(OPTION_TIME):
			if (timeSetState == SETHOUR) {
				tempTime.Hour = tempValue;
				tempTime.Minute = clockTime->Minute;
				tempTime.Second = clockTime->Second;
				rtcClock->write(tempTime);
				this->returnToStandby();
			}
			else if (timeSetState == SETMIN) {
				tempTime.Hour = clockTime->Hour;
				tempTime.Minute = tempValue;
				tempTime.Second = clockTime->Second;
				rtcClock->write(tempTime);
				this->returnToStandby();
			}
			else if (timeSetState == SETSEC) {
				tempTime.Hour = clockTime->Hour;
				tempTime.Minute = clockTime->Minute;
				tempTime.Second = tempValue;
				rtcClock->write(tempTime);
				this->returnToStandby();
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
			//After seconds, sort the times, find the next feed time, and exit time adjustment and return to standby menu state
			if (timeSetState == SETHOUR) {
				this->setFeedExist(feedDataPosition, true);
				feedData[feedDataPosition].time.Hour = tempValue;
				timeSetState = SETMIN;
				tempValue = 0;
			}
			else if (timeSetState == SETMIN) {
				feedData[feedDataPosition].time.Minute = tempValue;
				timeSetState = SETSEC;
				tempValue = 0;
			}
			else if (timeSetState == SETSEC) {
				feedData[feedDataPosition].time.Second = tempValue;
				this->bubbleSortFeedData();
				//feedtime changed, save data!
				this->saveData();
				this->returnToStandby();
			}
			break;
		}
	}
	else if (volumeSetState != OUTSIDE_VOLUME) {
		int feedDataPos;
		if (optionState == STATE1) {
			feedDataPos = 0;
		}
		else if (optionState == STATE2) {
			feedDataPos = 1;
		}
		else if (optionState == STATE3) {
			feedDataPos = 2;
		}
		else if (optionState == STATE4) {
			feedDataPos = 3;
		}
		if (volumeSetState == SETWHOLEDIGIT) {
			feedData[feedDataPos].volume = tempValue;
			tempValue = 0;
			volumeSetState = SETPARTIALDIGIT;
		}
		else {
			feedData[feedDataPos].volume += tempValue / (double)100;
			//volumed changed, save data!
			this->saveData();
			this->returnToStandby();
		}
	}
	else if (debugSetState != OUTSIDE_DEBUG) {
		switch (debugSetState) {
		case(DEBUG_MOTORTIME):
			feederSignalPacket.feederSignal = RUN_BYTIME;
			feederSignalPacket.Val = (double)tempValue;
			this->returnToStandby();
			break;
		case(DEBUG_MOTORVOLUME):
			feederSignalPacket.feederSignal = RUN_BYVOLUME;
			feederSignalPacket.Val = 100 + tempValue * 10;
			this->returnToStandby();
			break;
		default:
			this->returnToStandby();
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
				this->setFeedVolume(0, 0);
				this->setFeedExist(0, false);
				//Always bubblesort after changing feed data!!
				this->saveData();
				this->bubbleSortFeedData();
				optionState = STATE1;
			}
			else if (optionState == STATE3) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE4) {
				this->setFeedTime(1, 0, 0, 0);
				this->setFeedVolume(1, 0);
				this->setFeedExist(1, false);
				this->saveData();
				this->bubbleSortFeedData();
				optionState = STATE1;
			}
			else if (optionState == STATE5) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE6) {
				this->setFeedTime(2, 0, 0, 0);
				this->setFeedVolume(2, 0);
				this->setFeedExist(2, false);
				this->saveData();
				this->bubbleSortFeedData();
				optionState = STATE1;
			}
			else if (optionState == STATE7) {
				timeSetState = SETHOUR;
			}
			else if (optionState == STATE8) {
				this->setFeedTime(3, 0, 0, 0);
				this->setFeedVolume(3, 0);
				this->setFeedExist(3, false);
				this->saveData();
				this->bubbleSortFeedData();
				optionState = STATE1;
			}
			else if (optionState == STATE9) {
				this->returnToStandby();
			}
			break;
		case OPTION_FEEDVOLUME:
			if (optionState != STATE5) {
				volumeSetState = SETWHOLEDIGIT;
			}
			else {
				this->returnToStandby();
			}
			break;
		case OPTION_DEBUG:
			//exit state
			if (optionState == STATE3) {
				this->returnToStandby();
			}
			else if (optionState == STATE1) {
				debugSetState = DEBUG_MOTORTIME;
			}
			else if (optionState == STATE2) {
				debugSetState = DEBUG_MOTORVOLUME;
			}
			/*else if (optionState == STATE3) {
				debugSetState = DEBUG_ENCODERPOSITION;
			}
			else if (optionState == STATE4) {
				debugSetState = DEBUG_IRSENSOR;
			}*/
			break;
		}
	}
	//if the menu is displaying the available options
	else {
		switch (menuState) {
		case OPTION_TIME:
			optionState = STATE1;
			break;
		case OPTION_FEEDTIME:
			optionState = STATE1;
			break;
		case OPTION_FEEDVOLUME:
			if (feedData[0].exist) {
				optionState = STATE1;
			}
			else {
				optionState = STATE5;
			}
			break;
		case OPTION_EXIT:
			menuState = STANDBY;
			optionState = OUTSIDE;
			timeSetState = OUTSIDE_TIME;
			break;
		case OPTION_DEBUG:
			optionState = STATE1;
			break;
		}
	}
}
void Menu::setFeedTime(int feedPosition, int hour, int min, int sec) {
	feedData[feedPosition].time.Hour = hour;
	feedData[feedPosition].time.Minute = min;
	feedData[feedPosition].time.Second = sec;
}
void Menu::setFeedVolume(int feedPosition, double volume) {
	feedData[feedPosition].volume = volume;
}
void Menu::setFeedExist(int feedPosition, bool existState) {
	feedData[feedPosition].exist = existState;
}
void Menu::passClock(DS3231_Simple* rtcClock) {
	//this pointer is necessary so that when the user sets the feed time through the menu
	//this menu class is able to adjust the DS3231 clock
	Menu::rtcClock = rtcClock;
}
//These functions were made only for testing. !!Can be deleted when no longer necessary!!
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
//END OF TEST FUNCTIONS; DELETE LATER IF DESIRED
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
	//Print line with the current time on it
	lcd.setCursor(5, 0);
	lcd.print("TIME : ");
	lcd.setCursor(12, 0);
	if (clockTime->Hour >= 10) {
		lcd.print(clockTime->Hour);
	}
	else {
		lcd.setCursor(13, 0);
		lcd.print(clockTime->Hour);
	}
	lcd.setCursor(14, 0);
	lcd.print(":");

	lcd.setCursor(15, 0);
	if (clockTime->Minute >= 10) {
		lcd.print(clockTime->Minute);
	}
	else {
		lcd.print(0);
		lcd.setCursor(16, 0);
		lcd.print(clockTime->Minute);
	}
	lcd.setCursor(17, 0);
	lcd.print(":");

	lcd.setCursor(18, 0);
	if (clockTime->Second >= 10) {
		lcd.print(clockTime->Second);
	}
	else {
		lcd.print(0);
		lcd.setCursor(19, 0);
		lcd.print(clockTime->Second);
	}
	//print line that shows the user the next scheduled feed time
	lcd.setCursor(0, 1);
	lcd.print("NEXT FEED@: ");
	switch (nextFeedPos) {
	case -1:
		//no more feed times today
		lcd.setCursor(12, 1);
		lcd.print("TOMORR.");
		break;
	case -2:
		//user doesnt have any feed times set in memory!
		lcd.setCursor(12, 1);
		lcd.print("NONE SET");
		break;
	default:
		if (feedData[nextFeedPos].time.Hour >= 10) {
			lcd.setCursor(12, 1);
		}
		else {
			lcd.setCursor(13, 1);
		}
		lcd.print(feedData[nextFeedPos].time.Hour);
		lcd.setCursor(14, 1);
		lcd.print(":");
		lcd.setCursor(15, 1);
		if (feedData[nextFeedPos].time.Minute < 10) {
			lcd.print("0");
			lcd.setCursor(16, 1);
		}
		lcd.print(feedData[nextFeedPos].time.Minute);

		lcd.setCursor(17, 1);
		lcd.print(":");

		lcd.setCursor(18, 1);
		if (feedData[nextFeedPos].time.Second >= 10) {
			lcd.print(feedData[nextFeedPos].time.Second);
		}
		else {
			lcd.print(0);
			lcd.setCursor(19, 1);
			lcd.print(feedData[nextFeedPos].time.Second);
		}
	}

	//Print Load Line
	//Max of 15 total load bars
	lcd.setCursor(0, 3);
	lcd.print("LOAD:");
	//Fill Full Load Positions
	for (int i = 0; i < load; i++) {
		lcd.setCursor(5 + i, 3);
		lcd.write((uint8_t)0);
	}
	//Fill Empty load Positions
	for (int i = 19; i >= 5 + load; i--) {
		lcd.setCursor(i, 3);
		lcd.write((uint8_t)1);
	}
}
void Menu::printOption_Time() {
	//if in outer portion of time option part of menu
	if (timeSetState == OUTSIDE_TIME) {
		lcd.setCursor(4, 0);
		lcd.print("ADJUST TIME");
		lcd.setCursor(2, 1);
		//print the time
		if (clockTime->Hour >= 10) {
			lcd.print(clockTime->Hour);
		}
		else {
			lcd.setCursor(3, 1);
			lcd.print(clockTime->Hour);
		}
		lcd.setCursor(6, 1);
		lcd.print(":");

		lcd.setCursor(9, 1);
		if (clockTime->Minute >= 10) {
			lcd.print(clockTime->Minute);
		}
		else {
			lcd.print(0);
			lcd.setCursor(10, 1);
			lcd.print(clockTime->Minute);
		}
		lcd.setCursor(13, 1);
		lcd.print(":");

		lcd.setCursor(16, 1);
		if (clockTime->Second >= 10) {
			lcd.print(clockTime->Second);
		}
		else {
			lcd.print(0);
			lcd.setCursor(17, 1);
			lcd.print(clockTime->Second);
		}
		//places the cursor based on where the user is currently selecting
		//with the rotary encoder
		switch (optionState) {
		case STATE1:
			lcd.setCursor(3, 2);
			lcd.write((uint8_t)2);
			lcd.setCursor(3, 3);
			lcd.print("|");
			break;
		case STATE2:
			lcd.setCursor(10, 2);
			lcd.write((uint8_t)2);
			lcd.setCursor(10, 3);
			lcd.print("|");
			break;
		case STATE3:
			lcd.setCursor(17, 2);
			lcd.write((uint8_t)2);
			lcd.setCursor(17, 3);
			lcd.print("|");
			break;
		case STATE4:
			lcd.setCursor(15, 3);
			lcd.print("EXIT");
			break;
		}
	}
	//the user is in the second level part of the time option menu
	//where the user sets the individual values for the times hour/min/sec
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
			if ((i + tempValue) >= maxTimeDigit) {
				lcd.print(i + tempValue - maxTimeDigit);
			}
			else {
				lcd.print(i + tempValue);
			}
		}
		for (int i = 0; i > -4; i--) {
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempValue) < 0) {
				lcd.print(maxTimeDigit + i + tempValue);
			}
			else {
				lcd.print(i + tempValue);
			}
		}

		lcd.setCursor(9, 2);
		lcd.write((uint8_t)2);
		lcd.setCursor(9, 3);
		lcd.print("|");
	}
}
void Menu::printOption_Feedtime() {
	int numFeedTimes = 4;
	//In the outer portion othe feedtime menu
	if (timeSetState == OUTSIDE_TIME) {
		//user is indicating they wish to exit this part of the menu
		if (optionState == STATE9) {
			lcd.setCursor(8, 1);
			lcd.print("Exit");
		}
		else {
			for (int i = 0; i < numFeedTimes; i++) {
				//while printing lines if it finds an array value where the feedtime doesnt exist,
				//stop printing, because bubble sort ensures all feedtimes after it also do not exist
				if (feedData[i].exist == false) {
					break;
				}
				lcd.setCursor(7, i);
				lcd.print("T :");
				lcd.setCursor(8, i);
				lcd.print(i + 1);
				if (feedData[i].time.Hour >= 10) {
					lcd.setCursor(11, i);
				}
				else {
					lcd.setCursor(12, i);
				}
				lcd.print(feedData[i].time.Hour);
				lcd.setCursor(13, i);
				lcd.print(":");
				if (feedData[i].time.Minute >= 10) {
					lcd.setCursor(14, i);
				}
				else {
					lcd.setCursor(15, i);
				}
				lcd.print(feedData[i].time.Minute);
				lcd.setCursor(16, i);
				lcd.print(":");
				if (feedData[i].time.Second >= 10) {
					lcd.setCursor(17, i);
				}
				else {
					lcd.setCursor(18, i);
				}
				lcd.print(feedData[i].time.Second);

			}
			//places cursor based on state that is set with rotary encoder
			//"set" indicates the user can set the time on a button click
			//"rem" indicates to the user that they can rem that time on a button click
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
	//User wishes to change the feedtime at that position
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
			if ((i + tempValue) >= maxTimeDigit) {
				lcd.print(i + tempValue - maxTimeDigit);
			}
			else {
				lcd.print(i + tempValue);
			}
		}
		for (int i = 0; i > -4; i--) {
			lcd.setCursor(9 + (i * 3), 1);
			if ((i + tempValue) < 0) {
				lcd.print(maxTimeDigit + i + tempValue);
			}
			else {
				lcd.print(i + tempValue);
			}
		}

		lcd.setCursor(9, 2);
		lcd.write((uint8_t)2);
		lcd.setCursor(9, 3);
		lcd.print("|");
	}
}
void Menu::printOption_FeedVolume() {
	//in the first layer of the volume set menu
	if (volumeSetState == OUTSIDE_VOLUME) {
		if (optionState == STATE5) {
			lcd.setCursor(8, 1);
			lcd.print("Exit");
		}
		else {
			int numFeedTimes = 4;

			for (int i = 0; i < numFeedTimes; i++) {
				//bubble sort ensures that all feed times after a nonexistent feedtime in the array
				//also do not exist, so stop printing lines if a nonexistent feedtime is found
				if (feedData[i].exist == false) {
					break;
				}
				lcd.setCursor(3, i);
				lcd.print("F.Vol. :");
				lcd.setCursor(9, i);
				lcd.print(i);
				lcd.setCursor(12, i);
				lcd.print(feedData[i].volume);
				lcd.setCursor(16, i);
				lcd.print("cups");
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
	}
	//Inner menu of the feedvolume option
	else {
		int maxValue;
		//User is setting the integer value of the cups for the feed volume
		if (volumeSetState == SETWHOLEDIGIT) {
			maxValue = 10;
			lcd.setCursor(2, 0);
			lcd.print("SET WHOLE DIGIT:");
			for (int i = 0; i < 4; i++) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) >= maxValue) {
					lcd.print(i + tempValue - maxValue);
				}
				else {
					lcd.print(i + tempValue);
				}
			}
			for (int i = 0; i > -4; i--) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) < 0) {
					lcd.print(maxValue + i + tempValue);
				}
				else {
					lcd.print(i + tempValue);
				}
			}

			lcd.setCursor(9, 2);
			//lcd.write((uint8_t)2);
			lcd.setCursor(9, 3);
			lcd.print("|");
		}
		//after the user sets the integer value for the feed volume
		//they can set the partial value to the hundreths place
		else {
			maxValue = 99;
			lcd.setCursor(2, 0);
			lcd.print("SET PARTIAL DIGIT");
			for (int i = 0; i < 4; i++) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) >= maxValue) {
					lcd.print(i + tempValue - maxValue);
				}
				else {
					lcd.print(i + tempValue);
				}
			}
			for (int i = 0; i > -4; i--) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) < 0) {
					lcd.print(maxValue + i + tempValue);
				}
				else {
					lcd.print(i + tempValue);
				}
			}

			lcd.setCursor(9, 2);
			//lcd.write((uint8_t)2);
			lcd.setCursor(9, 3);
			lcd.print("|");
		}
	}
}
void Menu::printOption_PrintDebug() {
	//user is in the first layer of the debug menu
	if (debugSetState == OUTSIDE_DEBUG) {
		//user places the rotary encoder on the portion of the debug menu indicating they wish to exit
		if (optionState == STATE3) {
			lcd.setCursor(8, 1);
			lcd.print("Exit");
		}
		else {
			lcd.setCursor(4, 0);
			lcd.print("RunMotorByTime");
			lcd.setCursor(4, 1);
			lcd.print("RunMotorByDeg");
			/*lcd.setCursor(4, 2);
			lcd.print("Encod. Position");
			lcd.setCursor(4, 3);
			lcd.print("IR. Value");*/

			switch (optionState) {
			case STATE1:
				lcd.setCursor(0, 0);
				lcd.print("->");
				break;
			case STATE2:
				lcd.setCursor(0, 1);
				lcd.print("->");
				break;
			/*case STATE3:
				lcd.setCursor(0, 2);
				lcd.print("->");
				break;
			case STATE4:
				lcd.setCursor(0, 3);
				lcd.print("->");
				break;*/
			}
		}
	}
	//inner debug menu display
	else {
		//print based on the debug selection the user chose
		switch (debugSetState) {
		case DEBUG_MOTORTIME:
			lcd.setCursor(1, 0);
			lcd.print("MOTOR RUNTIME (sec)");
			for (int i = 0; i < 4; i++) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) > 20) {
					lcd.print(i + tempValue - 21);
				}
				else {
					lcd.print(i + tempValue);
				}
			}
			for (int i = 0; i > -4; i--) {
				lcd.setCursor(9 + (i * 3), 1);
				if ((i + tempValue) < 0) {
					lcd.print(21 + i + tempValue);
				}
				else {
					lcd.print(i + tempValue);
				}
			}
			lcd.setCursor(9, 2);
			//lcd.write((uint8_t)2);
			lcd.setCursor(9, 3);
			lcd.print("|");
			break;
		case DEBUG_MOTORVOLUME:
			lcd.setCursor(1, 0);
			lcd.print("DEGREES TO TURN:");
			lcd.setCursor(7, 1);
			lcd.print((int)((double)90 + (double)tempValue * 90 + (double)90));
			lcd.setCursor(4, 2);
			lcd.print("->");
			lcd.setCursor(7, 2);
			lcd.print((int)(double(90) + (double)tempValue * 90));
			if (tempValue != -10) {
				lcd.setCursor(7, 3);
				lcd.print((int)(double(90) + (double)tempValue * 90 - (double)90));
			}
			break;
		/*case DEBUG_ENCODERPOSITION:
			lcd.setCursor(3, 0);
			lcd.print("ENCODER POSITION:");
			lcd.setCursor(0, 2);
			lcd.print("Degree: ");
			break;
		case DEBUG_IRSENSOR:
			lcd.setCursor(2, 0);
			lcd.print("-IR INFORMATION-");
			lcd.setCursor(0, 1);
			lcd.print("IR ANALOG VAL:");
			lcd.setCursor(0, 2);
			lcd.print("CALC. HEIGHT: ");
			break;*/
		}
	}
}
void Menu::flagReset() {
	resetFlag = true;
}
FeederSignalPacket* Menu::recieveSignalPointer() {
	return &feederSignalPacket;
}
void Menu::signalRecieved() {
	feederSignalPacket.feederSignal = NOSIGNAL;
	feederSignalPacket.Val = 0;
}
void Menu::dispenseMessage(long encoderDegree, float turns, int timeRemaining) {
	this->clearScreen();
	lcd.setCursor(1, 0);
	lcd.print("--DISPENSING FOOD--");
	lcd.setCursor(0, 2);
	lcd.print("Encod.Ang.:");
	lcd.setCursor(17, 2);
	lcd.print("deg");
	lcd.setCursor(11, 2);
	lcd.print(encoderDegree);
	lcd.setCursor(0, 3);
	lcd.print("Calc.Turns: ");
	lcd.setCursor(15, 3);
	lcd.print("turns");
	lcd.setCursor(12, 3);
	lcd.print(turns);
	if (timeRemaining != -1) {
		//this only shows up if the user is running in the run-by-time option
		//in the debug menu
		lcd.setCursor(0, 1);
		lcd.print("Time Left: ");
		lcd.setCursor(10, 1);
		lcd.print(timeRemaining);
		lcd.setCursor(13, 1);
		lcd.print("secs");
	}
}
void Menu::flagUpdate(UserInput userinput) {
	resetFlag = true;
	if (userinput == LEFT) {
		inputFlag = USERLEFT;
	}
	else {
		inputFlag = USERRIGHT;
	}
}
void Menu::update(UserInput userInput) {
	//If the clock time is greater then the next feed time, the menu signals back to the main program
	//that it should dispense food using this pointer, which is checked every time the main loop runs for changes
	//if the nextfeedpos is < 0, this indicates either:
	//1)no feedtimes set by the user or,
	//2)The next feed time is during the subsequent day,
	//so the motor shouldn't be run if the nextFeedPos is any of those values
	if (nextFeedPos >= 0 && menuState != OPTION_FEEDTIME) {
		if (clockTime > feedData[nextFeedPos].time) {
			feederSignalPacket.feederSignal = RUN_BYVOLUME;
			feederSignalPacket.Val = feedData[nextFeedPos].volume;
			findNextFeed();
		}
	}
	//find the next feed time if its the start of a new day
	//(this is just a quick and easy brute force method to ensure that
	//the next feed time is found on the new day)
	if (clockTime->Hour == 0 && clockTime->Minute == 0) {
		findNextFeed();
	}
	if (inputFlag != NOINPUT) {
		switch (inputFlag) {
		case(USERLEFT):
			userInput = LEFT;
			inputFlag = NOINPUT;
			break;
		case(USERRIGHT):
			userInput = RIGHT;
			inputFlag = NOINPUT;
			break;
		}
	}
	//If the user has not used the menu for 10 seconds, return to standby state
	if (userInput == NONE && menuState != STANDBY && clockTime > (lastInputTime + 10)) {
		this->returnToStandby();
	}
	//if there is user input
	else if (userInput != NONE) {
		resetFlag = true;
		lastInputTime << clockTime;
		//Enters the option menu on user input while on the standby menu
		if (menuState == STANDBY) {
			menuState = OPTION_TIME;
		}
		else if (userInput == LEFT) {
			menuChoiceDecrement();
		}
		else if (userInput == RIGHT) {
			menuChoiceIncrement();
		}
		else if (userInput == BUTTON) {
			buttonPush();
		}
	}
	//Resets the screen when reset flags are up
	if (resetFlag == true) {
		delay(75);
		this->clearScreen();
		resetFlag = false;
	}
	//Prints the standby menu state if standby flag is raised
	if (menuState == STANDBY) {
		this->printStandby();
	}
	//If not on standby, and outside a particular option, print the entire option menu
	else if (optionState == OUTSIDE) {
		this->printOptions();
	}
	//Otherwise you're viewing a particular option, print that options respective menu
	else {
		switch (menuState) {
		case OPTION_TIME:
			this->printOption_Time();
			break;
		case OPTION_FEEDTIME:
			this->printOption_Feedtime();
			break;
		case OPTION_FEEDVOLUME:
			this->printOption_FeedVolume();
			break;
		case OPTION_DEBUG:
			this->printOption_PrintDebug();
			break;
		}
	}
}
bool operator>(const DateTime& time1, const DateTime& time2) {
	if (time1.Hour > time2.Hour) {
		return true;
	}
	else if (time1.Hour == time2.Hour && time1.Minute > time2.Minute) {
		return true;
	}
	else if (time1.Hour == time2.Hour && time1.Minute == time2.Minute && time1.Second > time2.Second) {
		return true;
	}
	else {
		return false;
	}
}
bool operator>(const DateTime* time1, const DateTime& time2) {
	if (time1->Hour > time2.Hour) {
		return true;
	}
	else if (time1->Hour == time2.Hour && time1->Minute > time2.Minute) {
		return true;
	}
	else if (time1->Hour == time2.Hour && time1->Minute == time2.Minute && time1->Second > time2.Second) {
		return true;
	}
	else {
		return false;
	}
}
DateTime operator+(const DateTime& time1, const int rightsum) {
	DateTime returntime;
	returntime.Second = time1.Second + rightsum;
	returntime.Minute = time1.Minute;
	returntime.Hour = time1.Hour;
	bool pass = false;
	while (pass == false) {
		if (returntime.Second >= 60) {
			returntime.Minute += 1;
			returntime.Second -= 60;
		}
		else {
			pass = true;
		}
	}
	pass = false;
	while (pass == false) {
		if (returntime.Minute >= 60) {
			returntime.Hour += 1;
			returntime.Minute -= 60;
		}
		else {
			pass = true;
		}
	}
	pass = false;
	while (pass == false) {
		if (returntime.Hour >= 24) {
			returntime.Hour -= 24;
		}
		else {
			pass = true;
		}
	}
	return returntime;
}
void operator<<(DateTime& time1, const DateTime* time2) {
	time1.Second = time2->Second;
	time1.Minute = time2->Minute;
	time1.Hour = time2->Hour;
}