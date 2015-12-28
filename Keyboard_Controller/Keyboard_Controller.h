enum DisplayArea { 
  line1, line2, line1left, line1right, line2left, line2right };

/* display text on the given display area */
void display(DisplayArea area, const char *  text);
void display(DisplayArea area, const char *  text1, const char * text2);
void display(DisplayArea area, const char *  text, int value);

/*--------------------------------- persistent presets ---------------------------------*/

class Sound {
public:
  byte bank;
  byte program_number;
  byte volume;
  byte reverb_send;
  byte effects_send;
  byte reserved1;
  byte reserved2;
  byte reserved3;
  byte reserved4;
  byte reserved5;
  byte reserved6;
  byte reserved7;
  byte reserved8;
  byte reserved9;
};

class Preset {
public:
  byte split_point;
  Sound right, left, foot;
};

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








