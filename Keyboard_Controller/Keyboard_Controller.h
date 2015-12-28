enum DisplayArea { 
  line1, line2, line1left, line1right, line2left, line2right };

/* display text on the given display area */
void display(DisplayArea area, const char *  text);
void display(DisplayArea area, const char *  text1, const char * text2);
void display(DisplayArea area, const char *  text, int value);

/*--------------------------------- state event machine ---------------------------------*/

enum State {
  playing, // normal state after power on
  selectSound 
};

enum Event {
  enterBtn, exitBtn, modWheel, pitchWheel, dial
};

void process(Event event);
void process(Event event, int value);







