/*
Name:    AutomatedFeeder.ino
Created: 3/8/2018 8:48:42 PM
Author:  Morg
*/

#include "Wire.h"
#include <DS3231_Simple.h>
#include <LiquidCrystal_I2C.h>
#include <Rotary.h>
#include "menu.h"
//#include "AS5040.h"
//class not used but code directly integrated into 
//readEncoder() in main 
#include "Servo.h"

//MOTOR ENCODER PINS
const int ENCODERDATAPIN = 10;
const int ENCODERCLOCKPIN = 9;
const int ENCODERCHIPSELECTPIN = 8;
//ROTARY PINS
const int ROTARY_PIN_SW = 4;
const int ROTARY_PIN_CLK = 2;
const int ROTARY_PIN_DT = 3;
//SERVO PIN(s)
const int SERVO_PIN = 11;
//Values to send to IR sensor at the respective states.
//These are *dummy* values, the true values need to be found
//experimentally
const int SERVO_DOOROPEN = 20;
const int SERVO_DOORCLOSED = 180;
const int SERVO_DOORDELAY = 1500;
//MOTOR PIN
const int MOTORPIN = 5;
//IR SENSOR
const int IRSENSOR = A1;
//IR SENSOR max and min analog readings and their corresponding height measurements
//as well as the max volume with the derived volume calculation.
const int IR_MAXLOADVALUE = 600;
const float IR_MAXLOADHEIGHT_INCH = 10.75;
const int IR_MINLOADVALUE = 40;
const float IR_MINLOADHEIGHT_INCH = 0;
const float MAXVOLUME = 909.8;
//This is the maximum amount of bars that indicate food level
//that can be displayed on the LCD screen.
const int MAXLOADBARS = 15;

DateTime time;
FeederSignalPacket* feederSignalPacket;

Servo myServo;
Rotary rotary(ROTARY_PIN_DT, ROTARY_PIN_CLK);
Menu *menu;
DS3231_Simple clock;

//for debouncing the button
boolean lastButton = HIGH;
boolean currentButton = HIGH;

//rotary encoder movement triggers interrupts, those interrupts
//call this function, which tells the menu which direction the user is turning
//the encoder
void checkUserInput() {
	char result = rotary.process();
	if (result == DIR_CW) {
		menu->flagUpdate(LEFT);
	}
	else if (result == DIR_CCW) {
		menu->flagUpdate(RIGHT);
	}
}

boolean debounce(int buttonpin) {
	boolean state1;
	boolean state2;
	boolean state3;
	do {
		state1 = digitalRead(buttonpin);
		delay(5);
		state2 = digitalRead(buttonpin);
		delay(5);
		state3 = digitalRead(buttonpin);
		delay(5);
	} while (state1 != state2 || state1 != state3);
	return state1;
}

long readEncoder() {
	int _data = ENCODERDATAPIN;
	int _clock = ENCODERCLOCKPIN;
	int _cs = ENCODERCHIPSELECTPIN;
	uint32_t raw_value = 0;
	uint16_t inputstream = 0;
	uint16_t c;
	digitalWrite(_cs, HIGH);
	digitalWrite(_clock, HIGH);
	delay(100);
	digitalWrite(_cs, LOW);
	delay(10);
	digitalWrite(_clock, LOW);
	delay(10);
	for (c = 0; c < 16; c++)
	{
		digitalWrite(_clock, HIGH);
		delay(10);
		inputstream = digitalRead(_data);
		raw_value = ((raw_value << 1) + inputstream);

		digitalWrite(_clock, LOW);
		delay(10);
	}
	raw_value = raw_value >> 6;
	raw_value = raw_value * 360 / 1024;
	return(raw_value);
}

int setFoodLevelByIR() {
	float temp = 0;
	//average the IR reading over 5 readings
	for (int i = 0; i < 5; i++) {
		temp += analogRead(IRSENSOR);
	}
	temp /= 5;
	temp = map(temp, IR_MINLOADVALUE, IR_MAXLOADVALUE, IR_MINLOADHEIGHT_INCH, IR_MAXLOADHEIGHT_INCH);
	temp = constrain(temp, IR_MINLOADHEIGHT_INCH, IR_MAXLOADHEIGHT_INCH);
	temp = (0.279) * pow(temp, 3) + (3.7125) * pow(temp, 2) + (12.4875) * temp;
	temp = temp / MAXVOLUME;
	temp = map(temp, 0, 100, 0, 15);
	temp = constrain(temp, 0, 15);
	menu->setLoad(temp);
}

int getFoodLevelInfo(int& sensorData, float& heightData, float& VolData) {
	//average the IR reading over 5 readings
	for (int i = 0; i < 5; i++) {
		sensorData += analogRead(IRSENSOR);
	}
	sensorData /= 5;
	heightData = map(sensorData, IR_MINLOADVALUE, IR_MAXLOADVALUE, IR_MINLOADHEIGHT_INCH, IR_MAXLOADHEIGHT_INCH);
	heightData = constrain(heightData, IR_MINLOADHEIGHT_INCH, IR_MAXLOADHEIGHT_INCH);
	VolData = (0.279) * pow(heightData, 3) + (3.7125) * pow(heightData, 2) + (12.4875) * heightData;
}

void motorOn() {
	digitalWrite(MOTORPIN, LOW);
}

void motorOff() {
	digitalWrite(MOTORPIN, HIGH);
}

void runMotorTime(int targetTime) {
	int currentDeg = readEncoder();
	int lastDeg = currentDeg;
	int totalDeg = 0;
	int time = 0;
	//open feeder doors before dispensing food
	myServo.write(SERVO_DOOROPEN);
	delay(SERVO_DOORDELAY);
	motorOn();
	menu->dispenseMessage(totalDeg, 0, targetTime);
	//turn the motor until the elapsed time has passed
	while (time < targetTime) {
		currentDeg = readEncoder();
		if (lastDeg > 300 && currentDeg < 100) {
			totalDeg += 360 - lastDeg + currentDeg;
			lastDeg = currentDeg;
		}
		else if (currentDeg > lastDeg) {
			totalDeg += currentDeg - lastDeg;
			lastDeg = currentDeg;
		}
		float turns = (float)totalDeg / (float)360;
		menu->dispenseMessage(totalDeg, turns, targetTime - time);
		time++;
		//delay of 560 is used to approximate one second through one iteration of the loop.
		//This is because encoderRead() has a combined delay of 440 coded into the function
		delay(560);
	}
	motorOff();
	//close doors after dispensing food
	delay(SERVO_DOORDELAY);
	myServo.write(SERVO_DOORCLOSED);
}

void runMotorDeg(int targetDegree) {
	int lastDeg = readEncoder();
	int currentDeg = readEncoder();
	int totalDeg = 0;
	myServo.write(SERVO_DOOROPEN);
	//open feeder doors before dispensing food
	delay(SERVO_DOORDELAY);
	motorOn();
	menu->dispenseMessage(totalDeg, 0);
	//turn the motor until the auger has rotated by the target
	//degree value
	while (totalDeg < targetDegree) {
		currentDeg = readEncoder();
		if (lastDeg > 300 && currentDeg < 100) {
			totalDeg += 360 - lastDeg + currentDeg;
			lastDeg = currentDeg;
		}
		else if (currentDeg > lastDeg) {
			totalDeg += currentDeg - lastDeg;
			lastDeg = currentDeg;
		}
		float turns = (float)totalDeg / (float)360;
		if (lastDeg != totalDeg) {
			menu->dispenseMessage(totalDeg, turns, -1);
		}
	}
	motorOff();
	//close doors after dispensing food
	delay(SERVO_DOORDELAY);
	myServo.write(SERVO_DOORCLOSED);
}

void setup() {
	menu = new Menu(&time);

	myServo.attach(SERVO_PIN);

	//Serial.begin(9600);
	//Encoder pins
	pinMode(ENCODERDATAPIN, INPUT);
	pinMode(ENCODERCLOCKPIN, OUTPUT);
	pinMode(ENCODERCHIPSELECTPIN, OUTPUT);
	//IR pins
	pinMode(IRSENSOR, INPUT);
	//Rotary button pin
	pinMode(ROTARY_PIN_SW, INPUT);
	//Motor pin
	pinMode(MOTORPIN, OUTPUT);

	//pointer used for recieving signals in the main loop
	feederSignalPacket = menu->recieveSignalPointer();

	//must get time before loading data
	clock.begin();
	setFoodLevelByIR();
	menu->passClock(&clock);
	time = clock.read();
	menu->loadData();
	motorOff;

	//interrupts for rotary encoder, rotary encoder must be on
	//pins 2 and 3
	attachInterrupt(0, checkUserInput, CHANGE);
	attachInterrupt(1, checkUserInput, CHANGE);
}

void loop() {
	time = clock.read();
	//Get button change information and send to menu
	currentButton = debounce(ROTARY_PIN_SW);
	if (lastButton == HIGH && currentButton == LOW) {
		menu->update(BUTTON);
	}
	else {
		menu->update();
	}
	lastButton = currentButton;
	//This code block checks if the menu is flagging a food dispensement event or
	//requesting a review of the IR sensor data. Will either run the motor, or gather the requested 
	//data and send it to the menu to display
	if (feederSignalPacket->feederSignal != NOSIGNAL) {
		//Do not allow interrupts to program while food is being dispensed
		detachInterrupt(0);
		detachInterrupt(1);
		if (feederSignalPacket->feederSignal == RUN_BYTIME) {
			runMotorTime(feederSignalPacket->Val);
		}
		else if (feederSignalPacket->feederSignal == RUN_BYDEG){
			runMotorDeg(feederSignalPacket->Val);
		}
		else if (feederSignalPacket->feederSignal == SHOW_IR) {
			int sensorVal = 0;
			float calcHeight = 0;
			float calcVol = 0;
			getFoodLevelInfo(sensorVal, calcHeight, calcVol);
			menu->printIRState(sensorVal, calcHeight, calcVol);
			delay(6000);
		}
		menu->flagReset();
		menu->signalRecieved();
		//set new food level
		setFoodLevelByIR();
		//reattach interrupts after motor run event;
		attachInterrupt(0, checkUserInput, CHANGE);
		attachInterrupt(1, checkUserInput, CHANGE);
	}
	//delay(100);
}