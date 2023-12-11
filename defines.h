// State Defines
//--------------------
#define STANDBY_MODE 0
#define INPUT_MODE 1
#define LOCKED_MODE 2
#define UNLOCKED_MODE 3
#define SETTINGS_MODE 4

// Error Defines
//--------------
#define SUCCESS 0
#define UNEXPECTED_ERROR 1
#define ILLEGAL_STATE_CHANGE 2
#define INVALID_INPUT 3
#define WRONG_PASSWORD 4
#define MAX_CHARACTERS_REACHED 5
#define ASCII_SUCCESS 6

// Keys
//-----
#define STANDBY_KEY 'A'
#define INPUT_KEY 'B'
#define UNLOCK_KEY 'C'
#define SETTINGS_KEY 'D'
#define ERASE_KEY '#'
#define SPECIAL_CHAR_KEY '*'

// Text Defines
//-------------
#define STANDBY_TEXT "STANDBY"
#define LOCKED_TEXT "LOCKED"
#define WARNING_MAX_CHARACTERS "Reached max characters!"
#define WARNING_INVALID_KEY "Invalid key!"
#define WARNING_ASCII "Invalid range, only <0-127>!"
#define DEFAULT_PASSWORD "122333"
#define ENTERING_SETTINGS_MODE "Entering Settings Mode!"
#define UNLOCKED_TEXT "SUCCESS"
#define TEXT_WRONG_PASSWORD "WRONG PASSWORD"
#define NULL_TERMINATION_STRING "\0"
#define NULL_TERMINATION_CHAR char(0)
// Numeric Defines
//----------------
#define TEMP_DELAY 1000
#define MAX_PASSWORD_LENGTH 32