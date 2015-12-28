#include <LiquidCrystal.h>
#include <MIDI.h>
#include "KetronSD2.h"
#include "Keyboard_Controller.h"

const char * MY_NAME = "Demian";

LiquidCrystal lcd(27,26,25,24,23,22);

// max. value of a MIDI controller
const int MIDI_CONTROLLER_MAX = 127;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, midi3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);

/* external switch 1 (damper pedal) pin, last known value and switch type opener/closer */
const int ext_switch_1_pin = 53;
int ext_switch_1_val;
boolean ext_switch_1_opener = false;
/* external switch 2 (pedal) pin, last known value and switch type opener/closer */
const int ext_switch_2_pin = 51;
int ext_switch_2_val;
boolean ext_switch_2_opener = false;
/* enter (red) button pin and last known value */
const int push_btn_enter_pin = 52;
int push_btn_enter_val;
/* exit (black) button pin and last known value */
const int push_btn_exit_pin = 50;
int push_btn_exit_val;
/* pitch bend wheel input pin and last known value */
const int pitch_wheel_pin = A0;
int pitch_wheel_val;
/* modulation wheel input pin and last known value */
const int mod_wheel_pin = A1;
int mod_wheel_val;
/* data entry dial */
const int data_entry_pin = A2;
int data_val;

/*--------------------------------- LCD display ---------------------------------*/

const int lcd_columns = 20;
const int lcd_rows = 2;

char buf[lcd_rows * lcd_columns + 1];

/* display text on the given display area */
void display(DisplayArea area, const char *  text) {
  display(area, text, "");
}

void display(DisplayArea area, const char * text, int value) {
  char buf2[8];
  sprintf(buf2, "%d", value);
  display(area, text, buf2);
}

void display(DisplayArea area, const char *  text1, const char * text2) {
  switch(area) {
  case line1:
    lcd.setCursor(0,0);
    break;
  case line2:
    lcd.setCursor(0,1);
    break;
    // TODO other positions, cut text
  }
  sprintf(buf, "%s %s", text1, text2);
  int i = strlen(buf);
  while (i < lcd_columns) {
    buf[i] = ' ';
    i++;
  }
  buf[i] = '\0';
  lcd.print(buf);
}

/*--------------------------------- setup and main loop ---------------------------------*/

int slice_counter = 0;
const int n_slices = 20;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(lcd_columns, lcd_rows);

  pinMode(ext_switch_1_pin, INPUT_PULLUP);
  pinMode(ext_switch_2_pin, INPUT_PULLUP);
  pinMode(push_btn_enter_pin, INPUT_PULLUP);
  pinMode(push_btn_exit_pin, INPUT_PULLUP);

  //  int channel = 1;
  int channel = MIDI_CHANNEL_OMNI;
  midi3.setHandleNoteOn(handleNoteOn);
  midi3.setHandleNoteOff(handleNoteOff);
  midi3.begin(channel);
  midi2.begin(channel);

  ext_switch_1_val = digitalRead(ext_switch_1_pin);
  ext_switch_1_opener = (ext_switch_1_val == LOW);
  ext_switch_2_val = digitalRead(ext_switch_2_pin);
  ext_switch_2_opener = (ext_switch_2_val == LOW);
  display(line1, "ext.sw.1:", ext_switch_1_opener?"opener":"closer");
  display(line2, "ext.sw.2:", ext_switch_2_opener?"opener":"closer");

  delay(1500);
}

void loop() {
  int inval;
  
  switch (slice_counter) {
    case 0:
      inval = digitalRead(push_btn_enter_pin);
      if (inval != push_btn_enter_val) {
        push_btn_enter_val = inval;
        if (inval == HIGH)
          process(enterBtn, inval);
      }
      break;
    case 2:
      inval = digitalRead(push_btn_exit_pin);
      if (inval != push_btn_exit_val) {
        push_btn_exit_val = inval;
        if (inval == HIGH)
          process(exitBtn, inval);
      }
      break;
    case 4:
    case 14:
      inval = digitalRead(ext_switch_1_pin);
      if (inval != ext_switch_1_val) 
        handleExtSwitch1(ext_switch_1_val = inval);
      break;
    case 6:
    case 16:
      inval = digitalRead(ext_switch_2_pin);
      if (inval != ext_switch_2_val)
        handleExtSwitch2(ext_switch_2_val = inval);
      break;
    case 8:
    case 18:
      inval = analogRead(pitch_wheel_pin);
      if (inval != pitch_wheel_val)
        handlePitchWheel(pitch_wheel_val = inval);
      break;
    case 10:
      inval = analogRead(mod_wheel_pin);
      if (inval != mod_wheel_val)
        handleModWheel(mod_wheel_val = inval);
      break;
    case 12:
      inval = analogRead(mod_wheel_pin);
      if (inval != mod_wheel_val)
        handleModWheel(mod_wheel_val = inval);
      break;
    default:
      // call this often
      midi3.read();
      //midi2.read();
  }
  slice_counter++;
  if (slice_counter >= n_slices)
    slice_counter = 0;
}

/*--------------------------------- state event machine ---------------------------------*/
/*

playing -- enter --> selectSound --> exit --> playing

*/

State state = playing;
int last_pitch_val = 0;
enum SD2Bank SD2_current_bank = Presets;
int program_number = 0;

void process(Event event, int value) {
  switch (state) {
    case playing:
      switch (event) {
        case enterBtn:
          state = selectSound;
          display(line1, "Sound");
          break;
      }
      break;
    case selectSound:
      switch (event) {
        case exitBtn:
          state = playing;
          display(line1, MY_NAME);
          break;
        case pitchWheel:
          if (value > 0 && last_pitch_val <= 0) {
            last_pitch_val = value;
            SD2_current_bank = nextBank(SD2_current_bank);
            midi3.sendControlChange(midi::BankSelect, SD2_current_bank, 1);
            midi3.sendProgramChange(program_number, 1);
          }
          else if (value < 0 && last_pitch_val >= 0) {
            last_pitch_val = value;
            SD2_current_bank = prevBank(SD2_current_bank);
            midi3.sendControlChange(midi::BankSelect, SD2_current_bank, 1);
            midi3.sendProgramChange(program_number, 1);
          }
          else if (value == 0) {
            last_pitch_val = value;
          }
          break;
        case modWheel:
          midi3.sendProgramChange(program_number = value, 1);
          break;
      }
      display(line2, toString(SD2_current_bank).c_str(), program_number);
      break;
  }
}

/*--------------------------------- external switches ---------------------------------*/

/**
 * Sends sustain on/off depending on the 
 * input voltage and on whether the external
 * switch (in the foot pedal) is an opener or closer.
 */
void handleExtSwitch1(int inval) {
  if (ext_switch_1_opener) 
    midi3.sendControlChange(midi::Sustain, inval == LOW ? 0 : 127, 1);
  else 
    midi3.sendControlChange(midi::Sustain, inval == HIGH ? 0 : 127, 1);
}

/**
 * Sends sostenuto on/off depending on the 
 * input voltage and on whether the external
 * switch (in the foot pedal) is an opener or closer.
 */
void handleExtSwitch2(int inval) {
  // TODO assignable controller number?
  if (ext_switch_2_opener) 
    midi3.sendControlChange(midi::Sostenuto, inval == LOW ? 0 : 127, 1);
  else 
    midi3.sendControlChange(midi::Sostenuto, inval == HIGH ? 0 : 127, 1);
}

/*--------------------------------- pitch bend wheel ---------------------------------*/

/* values from the A/D converter corresponding to wheel positions */
const unsigned int min_pitch = 376;
const unsigned int min_normal_pitch = 533;
const unsigned int lower_pitch_range = min_normal_pitch - min_pitch;
const unsigned int max_normal_pitch = 554;
const unsigned int max_pitch = 694;
const unsigned int upper_pitch_range = max_pitch - max_normal_pitch;
// force 1 initial MIDI message by initialization with invalid value
int pitch = MIDI_PITCHBEND_MAX + 1;

void handlePitchWheel(unsigned int inval) {
  //display(line1, "pitch", inval);
  unsigned long ulval; // avoids unsigned int overflow
  // TODO Achtung unsigned int overflow!
  if (inval >= min_normal_pitch) {
    if (inval <= max_normal_pitch) 
      inval = 0;
    else {
      ulval = MIDI_PITCHBEND_MAX;
      inval = (ulval * (inval - max_normal_pitch)) / upper_pitch_range;
      if (inval > MIDI_PITCHBEND_MAX)
        inval = MIDI_PITCHBEND_MAX;
    }
  } else {
    ulval = -MIDI_PITCHBEND_MIN;
    inval = (ulval * (min_normal_pitch - inval)) / lower_pitch_range;
    inval = -inval;
    if (inval < MIDI_PITCHBEND_MIN)
      inval = MIDI_PITCHBEND_MIN;
  }
  if (inval != pitch) {
    pitch = inval;
    //display(line1, "pitch ", inval);
    if (state == playing)
      midi3.sendPitchBend(pitch, 1);
    else 
      process(pitchWheel, pitch);
  }
}

/*--------------------------------- modulation wheel ---------------------------------*/

/* values from the A/D converter corresponding to wheel positions */
const unsigned int min_mod = 368;
const unsigned int max_mod = 650;
const unsigned int mod_range = max_mod - min_mod;
// force 1 initial MIDI message by initialization with invalid value
unsigned int modulation = MIDI_CONTROLLER_MAX + 1;

void handleModWheel(unsigned int inval) {
  //display(line2, "mod raw", inval);
  if (inval < min_mod)
    inval = 0;
  else
    inval -= min_mod;
  inval = (MIDI_CONTROLLER_MAX * inval) / mod_range;
  if (inval > MIDI_CONTROLLER_MAX)
    inval = MIDI_CONTROLLER_MAX;
  // controller value range is smaller than input value range
  // therefore different input values may mean the same controller value
  // avoid unnecessary MIDI messages, only send if controller value has changed
  if (inval != modulation) {
    modulation = inval;
    //display(line2, "mod", inval);
    if (state == playing)
      midi3.sendControlChange(midi::ModulationWheel, modulation, 1);
    else 
      process(modWheel, modulation);
  }
}

/*--------------------------------- note  on / note off ---------------------------------*/

void handleNoteOn(byte channel, byte note, byte velocity)
{
  /*
  lcd.setCursor(0,0);
  lcd.print("note on");
  lcd.print((int)channel);
  lcd.print(" nt");
  lcd.print((int)note);
  lcd.print(" vl");
  lcd.print((int)velocity);
  lcd.print("    ");
  */
}
void handleNoteOff(byte channel, byte note, byte velocity)
{
  /*
  lcd.setCursor(0,0);
  lcd.print("note off");
  lcd.print((int)channel);
  lcd.print(" nt");
  lcd.print((int)note);
  lcd.print(" vl");
  lcd.print((int)velocity);
  lcd.print("    ");
  */
}


