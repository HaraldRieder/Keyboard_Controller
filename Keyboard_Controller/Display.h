/*--------------------------------- LCD display ---------------------------------*/

LiquidCrystal lcd(27,26,25,24,23,22);

const int lcd_columns = 20;
const int lcd_rows = 2;

enum DisplayArea { 
  line1, line2, line1left, line1right, line2left, line2right };

/* display text on the given display area */
void display(DisplayArea area, const char *  text);
void display(DisplayArea area, int value);
void display(DisplayArea area, const char *  text1, const char * text2);
void display(DisplayArea area, const char *  text, int value);
void display(DisplayArea area, int value, const char *  text);

char buf[lcd_rows * lcd_columns + 1];

/* display text on the given display area */
void display(DisplayArea area, const char *  text) {
  display(area, text, "");
}

void display(DisplayArea area, int value) {
  char buf[8];
  sprintf(buf, "%d", value);
  display(area, buf, "");
}

void display(DisplayArea area, const char * text, int value) {
  char buf[8];
  sprintf(buf, "%d", value);
  display(area, text, buf);
}

void display(DisplayArea area, int value, const char * text) {
  char buf[8];
  sprintf(buf, "%d", value);
  display(area, buf, text);
}

void display(DisplayArea area, const char *  text1, const char * text2) {
  // defaults fitting to line1 area
  int columns = lcd_columns;
  int col = 0;
  int row = 0;
  switch(area) {
    case line2:
      row = 1;
      break;
    case line1left:
      columns /= 2;
      break;
    case line1right:
      columns /= 2;
      col = columns;
      break;
    case line2left:
      columns /= 2;
      row = 1;
      break;
    case line2right:
      columns /= 2;
      col = columns;
      row = 1;      
      break;
  }
  // clear area
  int i;
  for (i = 0; i < columns; i++) 
    buf[i] = ' ';
  buf[i] = '\0';
  lcd.setCursor(col, row);
  lcd.print(buf);
  // print text
  if (text2 == NULL || *text2 == '\0') 
    strncpy(buf, text1, columns);
  else
    sprintf(buf, "%s %s", text1, text2);
  int len = min(strlen(buf), columns);
  if (col == 0) 
    // left aligned
    lcd.setCursor(col, row);
  else
    // right aligned
    lcd.setCursor(col + columns - len, row);
  lcd.print(buf);
}

