#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <MIDI.h>
#include <MemoryFree.h>
#include "KetronSD2.h"
#include "Keyboard_Controller.h"

const char * MY_NAME = "DEMIAN";

LiquidCrystal lcd(27,26,25,24,23,22);

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

/* number of current preset, initially the first preset saved in EEPROM */
int preset_number = 0;
Preset currentPreset;

midi::Channel sound_channel = 1;
midi::Channel right_channel = 1, left_channel = 2, foot_channel = 3;

/*--------------------------------- LCD display ---------------------------------*/

const int lcd_columns = 20;
const int lcd_rows = 2;

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

void display(DisplayArea area, const char *  text1, const char * text2) {
  int columns = lcd_columns;
  switch(area) {
  case line1:
    lcd.setCursor(0,0);
    break;
  case line2:
    lcd.setCursor(0,1);
    break;
  case line1left:
    columns /= 2;
    lcd.setCursor(0,0);
    break;
  case line1right:
    columns /= 2;
    lcd.setCursor(10,0);
    break;
  case line2left:
    columns /= 2;
    lcd.setCursor(0,1);
    break;
  case line2right:
    columns /= 2;
    lcd.setCursor(10,1);
    break;
  }
  sprintf(buf, "%s %s", text1, text2);
  //int i = min(strlen(buf), columns);
  int i = strlen(buf);
  while (i < columns) {
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
  midi::Channel channel = MIDI_CHANNEL_OMNI;
  midi3.setHandleNoteOn(handleNoteOn);
  midi3.setHandleNoteOff(handleNoteOff);
  midi3.begin(channel);
//  midi2.begin(channel);

  ext_switch_1_val = digitalRead(ext_switch_1_pin);
  ext_switch_1_opener = (ext_switch_1_val == LOW);
  ext_switch_2_val = digitalRead(ext_switch_2_pin);
  ext_switch_2_opener = (ext_switch_2_val == LOW);
  //display(line1, "ext.sw.1:", ext_switch_1_opener?"opener":"closer");
  //display(line2, "ext.sw.2:", ext_switch_2_opener?"opener":"closer");

  const char * switch_modes;
  if (ext_switch_1_opener) {
    switch_modes = ext_switch_2_opener ? "1opn 2opn" : "1opn 2cls";
  } else {
    switch_modes = ext_switch_2_opener ? "1cls 2opn" : "1cls 2cls";
  }
  display(line1left, MY_NAME);
  display(line2left, switch_modes);
  display(line1right, freeMemory());
  display(line2right, "bytes free");
  
  readPreset(preset_number, currentPreset);
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
playing ---enter--> selectSound ---exit--> playing (selected sound)
selectSound ---enter---> selectPreset ---exit--> playing (selected preset)
*/

State state = playing;

/**
 * The state event machine for the user interface.
 * @param event user action
 * @value optional value, meaning depends on event type
 */
void process(Event event, int value) {
  static enum SD2Bank SD2_current_bank = SD2Presets;
  static int program_number = 0;

  switch (state) {

    case playing:
      switch (event) {
        case enterBtn:
          state = selectSound;
          sendSound(SD2_current_bank, program_number, sound_channel);
          displaySound(SD2_current_bank, program_number);
          return;
      }
      return;

    case selectSound:
      switch (event) {
        case exitBtn:
          state = playing;
          display(line1, MY_NAME);
          return;
        case enterBtn:
          state = selectPreset;
          sendPreset(currentPreset);
          displayPreset(currentPreset, preset_number);
          return;
        case pitchWheel:
          // sound select, increment/decrement by 1 or by 10 
          if (handlePitchWheelEvent(value, 0, MIDI_CONTROLLER_MAX, &program_number)) {
            midi3.sendProgramChange(program_number, sound_channel);
            displaySound(SD2_current_bank, program_number);
          }
          return;
        case modWheel:
          // bank select, program number remains unchanged
          value = value * n_SD2_banks / (MIDI_CONTROLLER_MAX+1);
          SD2Bank bank = toSD2Bank(value);
          if (bank != SD2_current_bank) {
            SD2_current_bank = bank;
            sendSound(SD2_current_bank, program_number, sound_channel);
            displaySound(SD2_current_bank, program_number);
          }
          return;
      }
      return;

    case selectPreset:
      switch (event) {
        case exitBtn:
          state = playing;
          display(line1left, MY_NAME);
          return;
        case pitchWheel:
          // preset select, increment/decrement by 1 or by 10 
          if (handlePitchWheelEvent(value, 0, n_presets-1, &preset_number)) {
            readPreset(preset_number, currentPreset);
            sendPreset(currentPreset);
            displayPreset(currentPreset, preset_number);
          }
          return;
        case modWheel:
          value = value * n_presets / (MIDI_CONTROLLER_MAX+1);
          if (value != preset_number) {
            preset_number = value;
            readPreset(preset_number, currentPreset);
            sendPreset(currentPreset);
            displayPreset(currentPreset, preset_number);
          }
          return;
      }
      return; 
  }
}

/**
 * Displays "Sound" in line 1 of the LCD.
 * Displays bank name, program number (starting with 1) and program name in line 2.
 */ 
void displaySound(SD2Bank bank, int number) {
  display(line1, "Sound");
  // display bank name and program number starting with 1
  display(line2left, toString(bank), number+1);
  // display program name
  display(line2right, toString(bank, number));
}

/**
 * Displays "Pres." and preset number (starting with 1) in line 1 on the left of the LCD.
 * Displays program names of right, left, foot in the 3 other areas of the LCD.
 */ 
void displayPreset(const Preset & preset, int number) {
  display(line1, "Preset", number+1);
  if (preset.foot.bank != invalid) 
    display(line1right, toString((SD2Bank)(preset.foot.bank), preset.foot.program_number));
  else 
    display(line1right, preset_number);
  if (preset.split_point != invalid) {
    display(line2left, toString((SD2Bank)(preset.left.bank), preset.left.program_number));
    display(line2right, toString((SD2Bank)(preset.right.bank), preset.right.program_number));
  }
  else if (preset.right.bank != invalid) 
    display(line2, toString((SD2Bank)(preset.right.bank), preset.right.program_number));
  else 
    display(line2, "Invalid Preset!");
}

/**
 * Sends bank select and program change MIDI messages on channel.
 * @param channel MIDI channel 1..16
 */
void sendSound(SD2Bank bank, midi::DataByte program_number, midi::Channel channel) {
    midi3.sendControlChange(midi::BankSelect, (midi::DataByte)bank, channel);
    midi3.sendProgramChange(program_number, channel);
}

/**
 * Sends sound settings of 1 preset part to MIDI.
 */ 
void sendSound(const Sound & sound, midi::Channel channel) {
  sendSound((SD2Bank)sound.bank, sound.program_number, channel);
  midi3.sendControlChange(midi::ChannelVolume, sound.volume, channel);
  midi3.sendControlChange(midi::Pan, sound.pan, channel);
  midi3.sendControlChange(0x5b, sound.reverb_send, channel); // SD-2, non-standard
  midi3.sendControlChange(0x5d, sound.effects_send, channel); // SD-2, non-standard
}

/**
 * Sends all sound settings of the given preset to MIDI.
 */
void sendPreset(const Preset & preset) {
  if (preset.foot.bank != invalid)
    sendSound(preset.foot, foot_channel); 
  if (preset.left.bank != invalid)
    sendSound(preset.left, left_channel); 
  if (preset.right.bank != invalid)
    sendSound(preset.right, right_channel); 
}

/**
 * increment/decrement target value by 1 or by 10 (depending on change of pb_value)
 * @param value pitch bend value from MIDI_PITCHBEND_MIN to MIDI_PITCHBEND_MAX
 * @param min_value minimum allowed value for target_value
 * @param max_value maximum allowed value for target_value
 * @param target_value to be incremented/decremented
 * @param true if target_value has been changed
 */
boolean handlePitchWheelEvent(int value, const int min_value, const int max_value, int * target_value) {
  static int last_pitch_val = 0;
  boolean changed = false;
  value /= (MIDI_PITCHBEND_MAX*3/10);
  // now value is in [-3,-2,-1,0,1,2,3]
  //display(line1, "value", value);
  switch(value) {
    case -3:
      if (last_pitch_val > value) {
        * target_value = min_value;
        changed = true;
      }
      break;
    case -2:
      if (last_pitch_val > value) {
        * target_value = max(* target_value - 10, min_value);
        changed = true;
      }
      break;
    case -1:
      if (last_pitch_val > value) {
        * target_value = max(* target_value - 1, min_value);
        changed = true;
      }
      break;
    case 1:
      if (last_pitch_val < value) {
        * target_value = min(* target_value + 1, max_value);
        changed = true;
      }
      break;
    case 2:
      if (last_pitch_val < value) {
        * target_value = min(* target_value + 10, max_value);
        changed = true;
      }
      break;
    case 3:
      if (last_pitch_val < value) {
        * target_value = max_value;
        changed = true;
      }
      break;
  }
  last_pitch_val = value;
  return changed;
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
}
void handleNoteOff(byte channel, byte note, byte velocity)
{
}


