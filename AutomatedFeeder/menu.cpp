#include "menu.h"
#include "LiquidCrystal.h"

//Constructor
//Initializes LCD and creates special characters
Menu::Menu(int EN, int RS, int D4, int D5, int D6, int D7){
	lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);
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

//Sets variable corresponding to hours till the next feed
void Menu::setETA(int hours) {
	Menu::hours = hours;
}

//sets variable corresponding till time till next feed
void Menu::setFeed(int time) {
	Menu::feedTime = time;
}

//These functions were made only for testing the motor encoder. Delete when not needed.
void Menu::testPrint(long test) {
	lcd.setCursor(0, 2);
	lcd.print(test);
}
void Menu::testPrint2(long test) {
	lcd.setCursor(6, 2);
	lcd.print(test);
}

//prints the line with information that tells user when the next estimated refill is
void Menu::printETA() {
	lcd.setCursor(0, 1);
	lcd.print("Refill ETA :");
	lcd.setCursor(13, 1);
	lcd.print(hours);
	lcd.setCursor(16, 1);
	lcd.print("Hrs");
}

//prints the line that informs when the animal will be fed next
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
