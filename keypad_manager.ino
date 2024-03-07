const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { 'D', 'C', 'B', 'A' },
  { '#', '9', '6', '3' },
  { '0', '8', '5', '2' },
  { '*', '7', '4', '1' }
};

const char* keyMap[10] = {
  " _0",
  "ABC1",
  "DEF2",
  "GHI3",
  "JKL4",
  "MNO5",
  "PQR6",
  "STU7",
  "VWX8",
  "YZ.9",
};

byte rowPins[ROWS] = { 16, 0, 2, 14 };  //Keypad pins
byte colPins[COLS] = { 12, 3, 10, 9 };

unsigned long lastKeyPressTime = 0;
char currentKey = '\0';
int charIndex = 0;
String currentWord = "";

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void handleKeyPress(char key);
void addToCurrentWord(char key);
void deleteLastCharacter();
void clearCurrentWord();
void submitCurrentWord();
void handleStop();
void refreshLCD();

void keypadManager() {
  char key = keypad.getKey();

  if (key) {
    handleKeyPress(key);
    refreshLCD();
  }
}

void handleKeyPress(char key) {
  switch (key) {
    case '*':
      submitCurrentWord();
      break;
    case '#':
      handleStop();
      break;
    case 'A':
      deleteLastCharacter();
      break;
    case 'B':
      clearCurrentWord();
      break;
    case 'D':
      currentState = STATE_SETUP;
      break;
    default:
      addToCurrentWord(key);
      break;
  }
}

void submitCurrentWord() {
  if (currentState == STATE_TRANSMITTING) {
    return;
  }
  captureInput(currentWord);
  clearCurrentWord();
}

void handleStop() {
  currentState = STATE_CONNECTING_WIFI;
}

void deleteLastCharacter() {
  currentWord.remove(currentWord.length() - 1);
  refreshLCD();
}

void clearCurrentWord() {
  currentWord = "";
  refreshLCD();
}

void addToCurrentWord(char key) {
  unsigned long currentTime = millis();

  if (key == currentKey && currentTime - lastKeyPressTime < 1000) {
    currentWord.remove(currentWord.length() - 1);
    charIndex = (charIndex + 1) % strlen(keyMap[key - '0']);
  } else {
    currentKey = key;
    charIndex = 0;
  }
  currentWord += keyMap[key - '0'][charIndex];
  lastKeyPressTime = currentTime;
}

void refreshLCD() {
  line_1 = currentWord;
  updateLCD();
}
