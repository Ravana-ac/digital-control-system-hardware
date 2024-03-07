#define LCD_WIDTH 16

void updateLCD() {

  if (p_line_0 != line_0) {
    p_line_0 = line_0;
    String displayLine0 = line_0.substring(0, LCD_WIDTH);
    lcd.setCursor(0, 0);
    lcd.print(displayLine0);
    for (int i = displayLine0.length(); i < LCD_WIDTH; i++) {
      lcd.print(" ");
    }
  }

  if (p_line_1 != line_1) {
    p_line_1 = line_1;
    String displayLine1 = line_1.substring(0, LCD_WIDTH);
    lcd.setCursor(0, 1);
    lcd.print(displayLine1);
    for (int i = displayLine1.length(); i < LCD_WIDTH; i++) {
      lcd.print(" ");
    }
  }
}
