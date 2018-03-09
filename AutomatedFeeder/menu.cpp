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

//Sets the load property of the class
void Menu::setLoad(int load) {
	if (load > 15) {
		load = 15;
	}
	if (load < 0) {
		load = 0;
	}
	Menu::load = load;
}

//Updates the screen based on all current information in the class
void Menu::update() {
	printLoad();
}
