#include "defines.h"
#include "EEPROM.h"

char gKey = 0;
char gEntry[MAX_PASSWORD_LENGTH];
char gPassword[MAX_PASSWORD_LENGTH];
int gTemp = 0;
int gDisplayCount = 0;
int gRowFlag = 0;
int gState = STANDBY_MODE;
int gAttempts = 0;
int gSpecialCharFlag = 0;
int gSizePassword = 0;

void initStateMachine()
{
	debugEEPROM();
	readPasswordFromEEPROM(gPassword);
	getPasswordSize(gPassword, &gSizePassword);
	
	while(1)
	{
		switch(gState)
		{
			case STANDBY_MODE:
			case UNLOCKED_MODE:
				getInput(&gKey);
				changeState(&gKey, &gState, gEntry, &gDisplayCount, &gRowFlag, gPassword);
				break;
			case INPUT_MODE:
			case SETTINGS_MODE:
				getInput(&gKey);
				if(printOnDisplay(&gKey, &gDisplayCount, &gRowFlag, gEntry) == SUCCESS)
				changeState(&gKey, &gState, gEntry, &gDisplayCount, &gRowFlag, gPassword);
				break;
			case LOCKED_MODE:
				break;
			
			default:
				break;
		}
	}
}

void getInput(char* key)
{
	*key = keypad.getKey();
}

int printOnDisplay(char* key, int* displayCount, int* rowFlag, char entry[])
{
	if(*key)
	{	
		// Entering character via ASCII code
		//----------------------------------
		if(gSpecialCharFlag)
		{			
			if(*key == STANDBY_KEY && *key == INPUT_KEY && *key == UNLOCK_KEY && *key == SETTINGS_KEY)
			{
				tempDisplayText(NULL_TERMINATION_STRING, WARNING_ASCII, displayCount);
				return INVALID_INPUT;
			}
			
			if(gSpecialCharFlag == 4 || *key == SPECIAL_CHAR_KEY)
			{
				gSpecialCharFlag = 0;
				
				lcd.clear();
				displayEntry(displayCount, entry);
				*key = (char) gTemp;
				printKey(key, displayCount, rowFlag, entry);
				
				gTemp = 0;
				
				return SUCCESS;
			}
			
			int tempKeyCharToInt = (*key - '0');
			
			switch(gSpecialCharFlag)
			{
				case(1):
					tempKeyCharToInt *= 10;
				case(2):
					tempKeyCharToInt *= 10;
				case(3):
					break;
				default:
					return UNEXPECTED_ERROR;
			}
			
			gTemp += tempKeyCharToInt;
			gSpecialCharFlag++;
			// ****** Just for debugging *******
			Serial.print("Specialchar call: ");
			Serial.println(*displayCount);
			// ****** Just for debugging *******
			
			printKey(key, displayCount, rowFlag, entry);
			return SUCCESS;
		}

		// Skip printing the key and attempt to switch state
		//--------------------------------------------------
		if(*key == STANDBY_KEY || *key == UNLOCK_KEY)
			return SUCCESS;
		
		// Clear screen on erase key
		//--------------------------
		if(*key == ERASE_KEY)
		{
			resetInputMode(displayCount, rowFlag, entry);
			
			lcd.clear();
			return SUCCESS;
		}
		

		
		/* // Everything ok --> print key
		//----------------------------
		lcd.print(*key);
		
		entry[*displayCount] = *key;
		*displayCount = *displayCount + 1;
		*key = 0; */
		
		printKey(key, displayCount, rowFlag, entry);
	}
	
	return SUCCESS;
}


int printKey(char* key, int* displayCount, int* rowFlag, char entry[])
{
	if(checkBeforePrintingKey(key, displayCount, rowFlag, entry) != SUCCESS)
		return UNEXPECTED_ERROR;
	
	lcd.print(*key);
	if(!gSpecialCharFlag)
	{
		entry[*displayCount] = *key;
		*displayCount = *displayCount + 1;
	}
	*key = 0;
	
	return SUCCESS;
}

int checkBeforePrintingKey(char *key, int *displayCount, int *rowFlag, char entry[])
{
	// ****** Just for debugging *******
	Serial.println(*displayCount);
	// ****** Just for debugging *******
	
	// Invalid buttons
	//----------------
	if(*key == INPUT_KEY || *key == SETTINGS_KEY)
	{
		tempDisplayText(entry, WARNING_INVALID_KEY, displayCount);
		return INVALID_INPUT;
	}
	
	// Warning if reached max characters
	//----------------------------------
	if(*displayCount >= MAX_PASSWORD_LENGTH && *rowFlag == 1)
	{
	   tempDisplayText(entry, WARNING_MAX_CHARACTERS, displayCount);
	   return MAX_CHARACTERS_REACHED; 
	}
	
	// Enter ASCII via numeric code
	//-----------------------------
	if(*key == SPECIAL_CHAR_KEY)
	{
		gSpecialCharFlag = 1;
		lcd.clear();
		return ASCII_SUCCESS;
	}
	
	// Hop on next line if first line is full
	//---------------------------------------
	if(*displayCount >= 16 && *rowFlag != 1 && gSpecialCharFlag == 0)
	{
		Serial.println("Test");
		lcd.setCursor(0,1);
		*rowFlag = 1;
	}
	
	return SUCCESS;
}

int checkPassword(char *input, char *password)
{
  if(*input != *password)
  {
    lcd.print("Incorrect password.");
    return SUCCESS;
  }
  else
  {
    return WRONG_PASSWORD;
  }
}

int changeState(char* key, int* state, char entry[], int* displayCount, int* rowFlag, char password[])
{
	// Switch to according state depending on key pressed
	//---------------------------------------------------
	switch(*state)
	{
		case STANDBY_MODE:
			if(*key != INPUT_KEY && *key != SETTINGS_KEY && *key != STANDBY_KEY && *key != 0)
			{
				int tmp = 7;
				tempDisplayText(STANDBY_TEXT, WARNING_INVALID_KEY, &tmp);
				return INVALID_INPUT;
			}
			else
			{
				if(*key == INPUT_KEY)
				{
					lcd.clear();
					debugEEPROM();
					*state = INPUT_MODE;
					return INPUT_MODE;
				}
			}
			break;
		case INPUT_MODE:
			if(*key == STANDBY_KEY)
			{
				enterStandbyMode(displayCount, rowFlag, entry);
				*state = STANDBY_MODE;
				return STANDBY_MODE;
			}
			if(*key == UNLOCK_KEY)
			{
				// ****** Just for debugging *******
				Serial.println(*displayCount);
				// ****** Just for debugging *******
				
				if(attemptUnlocking(entry, gPassword, displayCount) == SUCCESS)
				{
					tempDisplayText(NULL_TERMINATION_STRING, UNLOCKED_TEXT, displayCount);
					resetInputMode(displayCount, rowFlag, entry);
					*state = UNLOCKED_MODE;
					return UNLOCKED_MODE;
				}
				else
				{
					tempDisplayText(NULL_TERMINATION_STRING, TEXT_WRONG_PASSWORD, displayCount);
					resetInputMode(displayCount, rowFlag, entry);
					return INPUT_MODE;
				}
			}
			break;
		case UNLOCKED_MODE:
			if(*key == STANDBY_KEY)
			{
				enterStandbyMode(displayCount, rowFlag, entry);
				*state = STANDBY_MODE;
				return STANDBY_MODE;
			}
			if(*key == SETTINGS_KEY)
			{
				Serial.print("sett:");
				Serial.println(*displayCount);
				tempDisplayText(NULL_TERMINATION_STRING, ENTERING_SETTINGS_MODE, displayCount);
				resetInputMode(displayCount, rowFlag, entry);
				*state = SETTINGS_MODE;
				return SETTINGS_MODE;
			}
			break;
		case SETTINGS_MODE:
			if(*key == STANDBY_KEY)
			{
				savePassword(password, displayCount, entry);
				readPasswordFromEEPROM(gPassword);
				getPasswordSize(gPassword, &gSizePassword);
				enterStandbyMode(displayCount, rowFlag, entry);
				*state = STANDBY_MODE;
				return STANDBY_MODE;
			}
			break;
		case LOCKED_MODE:
			displayText(LOCKED_TEXT);
			break;
		default:
		  
		  break;
	}

  return UNEXPECTED_ERROR;
}

int enterStandbyMode(int* displayCount, int* rowFlag, char entry[])
{
	resetInputMode(displayCount, rowFlag, entry);
	displayText(STANDBY_TEXT);
	return SUCCESS;
}

void displayText(char string[])
{
	lcd.clear();
	lcd.print(string);
}

void tempDisplayText(char currentText[], char tempText[], int* count)
{
	lcd.clear();
	lcd.print(tempText);
	delay(TEMP_DELAY);
	lcd.clear();
	
	// ****** Just for debugging *******
	Serial.print("currentText[0]");
	Serial.println(currentText[0]);
	// ****** Just for debugging *******
	
	if(currentText[0] != char(0))
	{
		displayEntry(count, currentText);	
	}
}

int resetInputMode(int *displayCount, int *rowFlag, char entry[])
{
	*rowFlag = 0;
	*displayCount = 0;
	memset(entry, 0, sizeof(entry));
	
	return SUCCESS;
}

int displayEntry(int* displayCount, char entry[])
{
	// ****** Just for debugging *******
	Serial.print("Displaycount: ");
	Serial.println(*displayCount);
	// ****** Just for debugging *******
	
	for(int i = 0; i < *displayCount; i++)
	{
		if(i == 16)
		{
			lcd.setCursor(0,1);
		}
		lcd.print(entry[i]);
	}
}

int attemptUnlocking(char entry[], char password[], int* displayCount)
{
	int tempCheckDone = 0;
	
	if(gSizePassword != *displayCount)
	{
		// ****** Just for debugging *******
		Serial.print("Displaycount: ");
		Serial.print(*displayCount);
		Serial.print(" == gSizePassword: ");
		Serial.println(gSizePassword);
		// ****** Just for debugging *******
		
		return WRONG_PASSWORD;
	}
	
	for(int i = 0; i < *displayCount; i++)
	{
		tempCheckDone = 1;
		
		// ****** Just for debugging *******
		Serial.print("Entry: ");
		Serial.println(entry[i]);
		Serial.print("Password: ");
		Serial.println(password[i]);
		// ****** Just for debugging *******
		
		if(entry[i] != password[i])
		{
			return WRONG_PASSWORD;
		}
	}
	
	if(tempCheckDone)
	{
		return SUCCESS;
	}
}

int readPasswordFromEEPROM(char password[])
{
	for(int i = 0; i < MAX_PASSWORD_LENGTH; i++)
	{
		password[i] = (char)EEPROM.read(i);
		
		if(password[i] == NULL_TERMINATION_CHAR)
		{
			break;
		}
	}
	
	return SUCCESS;
}

int savePassword(char password[], int* displayCount, char entry[])
{
	for(int i = 0; i <= *displayCount; i++)
	{
		EEPROM.write(i, entry[i]);
		
		// Add Null Termination after password
		//------------------------------------
		if(i == *displayCount)
		{
			EEPROM.write(i, NULL_TERMINATION_CHAR);
		}
	}
	
	return SUCCESS;
}

void getPasswordSize(char password[], int* size)
{
	for(int i = 0; i < MAX_PASSWORD_LENGTH; i++)
	{
		// ****** Just for debugging *******
		Serial.print("Password: ");
		Serial.println(password[i]);
		// ****** Just for debugging *******
		
		if(password[i] == NULL_TERMINATION_CHAR)
		{
			*size = i;
			break;
		}
	}
}

void debugEEPROM()
{
	for(int i = 0; i < 30; i++)
	{
		Serial.print("EEPROM[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println((char)EEPROM.read(i));
	}		
}

void setDefaultPassword()
{
	EEPROM.write(0, '1');
	EEPROM.write(1, '2');
	EEPROM.write(2, '3');
	EEPROM.write(3, '3');
	EEPROM.write(4, NULL_TERMINATION_CHAR);
}