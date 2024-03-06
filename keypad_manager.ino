void keypad_manager() {
  char key = keypad.getKey();

  if (key) {

    switch (key) {
      case '*':
        {
          if (transmitStarted) {
            return;
          }
          capture_input(currentWord);
          currentWord = "";
          line_1 = currentWord;
          return;
        }
      case '#':
        {
          transmitStarted = true;
          capture_input(currentWord);
          return;
        }
      case 'A':
        {
          currentWord.remove(currentWord.length() - 1);
          line_1 = currentWord;
          return;
        }
      case 'B':
        {
          currentWord = "";
          line_1 = currentWord;
          update_lcd();
          return;
        }
      case 'C':
        {
          return;
        }
      case 'D':
        {
          return;
        }
    }

    unsigned long currentTime = millis();

    if (key == currentKey && currentTime - lastKeyPressTime < 1000) {
      currentWord.remove(currentWord.length() - 1);
      charIndex = (charIndex + 1) % strlen(keyMap[key - '0']);
    } else {
      currentKey = key;
      charIndex = 0;
    }

    currentWord += keyMap[key - '0'][charIndex];

    line_1 = currentWord;

    lastKeyPressTime = currentTime;
  }
}