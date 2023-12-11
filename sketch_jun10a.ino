#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>
#include "defines.h"

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4; // number of rows
const byte COLS = 4; // number of columns
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[ROWS] = {8, 9, 10, 11}; // row pinouts of the keypad R1 = D8, R2 = D7, R3 = D6, R4 = D5
byte colPins[COLS] = {14, 15, 16, 17};    // column pinouts of the keypad C1 = D4, C2 = D3, C3 = D2
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);



void setup()
{

  Serial.begin(9600);
  lcd.begin(16, 2); //Im Setup wird angegeben, wie viele Zeichen und Zeilen verwendet werden. Hier: 16 Zeichen in 2 Zeilen.
  lcd.setCursor(0,0);

void loop() 
{
	//displayText(TEXT);
	//delay(1000);

	initStateMachine();
	
	/* key = keypad.getKey(); 

	if (key)
	{
		getInput(&key, &displayCount, &rowFlag);
	}
	
	Serial.println(displayCount); */
}
