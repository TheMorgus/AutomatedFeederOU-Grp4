#include "menu.h"
#include "LiquidCrystal.h"

//Constructor
//Initializes LCD and creates special characters
Menu::Menu() {
	lcd.begin(20, 4);
	lcd.createChar(0, (uint8_t*)load_full);
	lcd.createChar(1, (uint8_t*)load_empty);
}

//Private Variable
//Prints the load line, with load bars corresponding to classes load property.
//Load Bar Max == 15;
void Menu::printLoad() {
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

void Menu::setETA(int hours) {
	Menu::hours = hours;
}

void Menu::setFeed(int time) {
	Menu::feedTime = time;
}

void Menu::printETA() {
	lcd.setCursor(0, 1);
	lcd.print("Refill ETA :");
	lcd.setCursor(13, 1);
	lcd.print(hours);
	lcd.setCursor(16, 1);
	lcd.print("Hrs");
}

void Menu::printFeed() {
	lcd.setCursor(0, 0);
	lcd.print("Next Feed @: ");
	lcd.setCursor(13, 0);
	lcd.print(feedTime);
}

//Updates the screen based on all current information in the class
void Menu::update() {
	printFeed();
	printETA();
	printLoad();
}
