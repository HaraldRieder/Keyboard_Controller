// max. value of a MIDI controller
const byte MIDI_CONTROLLER_MAX = 127;
const byte MIDI_CONTROLLER_MEAN = 64;

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

/*--------------------------------- persistent settings ---------------------------------*/

// the values in a fresh EEPROM
const byte invalid = 0xff;

/*--------- global settings ---------*/
// start address of global settings storage area in EEPROM
const int GlobalSettingsAddress = 0;

const int n_global_settings = 2;

enum GlobalParameter {
  BassBoostParam, BoostFreqParam
};

struct GlobalSettings {
  byte SD2_bass_boost;
  byte SD2_boost_freq;
  byte reserved3;
  byte reserved4;
  byte reserved5;
  byte reserved6;
  byte reserved7;
  byte reserved8;
  byte reserved9;
};

GlobalSettings globalSettings;

/**
 * Reads global settings from EEPROM.
 */
void readGlobals() {
  // TODO checksum and detection of corrupt data
  byte *b = (byte*)&globalSettings;
  for (int i = 0; i < sizeof(GlobalSettings); i++)
    b[i] = EEPROM.read(GlobalSettingsAddress + i);
  if (globalSettings.SD2_bass_boost > max_boost_gain)
    globalSettings.SD2_bass_boost = 0;
  if (globalSettings.SD2_boost_freq > max_boost_freq)
    globalSettings.SD2_boost_freq = max_boost_freq/2;
}

/**
 * Saves global settings to EEPROM. To maximize EEPROM lifetime only changed values are actually written. 
 */
void saveGlobals() {
  byte *b = (byte*)&globalSettings;
  for (int i = 0; i < sizeof(GlobalSettings); i++) {
    byte original = EEPROM.read(GlobalSettingsAddress + i);
    if (original != b[i])
      EEPROM.write(GlobalSettingsAddress + i, b[i]);
  }
}

/*--------- sound presets ---------*/
const int n_switch_assignable_ctrls = 6;
enum SwitchAssignableController {
  NoSwitch = invalid,
  Sustain = midi::Sustain, // values 00 or 7f
  Sostenuto = midi::Sostenuto, // values 00 or 7f
  Soft = midi::SoftPedal, // values 00 or 7f
  WhaWha = 0x54, // values 00 or 7f
  Rotor = 0x1e, // values 00 off or default?, 40 slow, 7f fast
};

const int n_wheel_assignable_ctrls = 6;
enum WheelAssignableController {
  NoWheel = invalid,
  Modulation = midi::ModulationWheel,
  WhaWhaAmount = 0x55,
  CutoffFrequency = 0x60, // NRPN, pseudo controller number!
  Resonance = 0x61, // NRPN, pseudo controller number! 
  Pitch = 0xfe // must be the last, only offered for pitch wheel
};

const int n_presets = 60; // 54 bytes/preset * 60 presets = 3240 bytes EEPROM occupied by presets
const int n_sounds_per_preset = 3; // foot + left hand + right hand

// start address of preset storage area in EEPROM
const int PresetsAddress = 40;

/* EEPROM structure */
struct Sound {
public:
  byte bank;
  byte program_number;
  byte transpose;
  byte volume;
  byte pan;
  byte reverb_send;
  byte effects_send;
  byte mod_wheel_ctrl_no;
  byte pitch_wheel_ctrl_no;
  byte ext_switch_1_ctrl_no;
  byte ext_switch_2_ctrl_no;
  byte cutoff_frequency;
  byte resonance;
  byte channel; // not [yet] adjustable but comfortable here
  byte reserved1;
  byte reserved2;
  byte reserved3;
};

const long magic = 0x20031fe1;

/* EEPROM structure */
struct Preset {
public:
  long magic;
  byte split_point;
  byte reserved6;
  byte reserved7;
  byte reserved8;
  byte reserved9;
  Sound right, left, foot;
};

void defaultPreset(Preset & preset);
void readPreset(int presetNumber, Preset & preset);
void savePreset(int presetNumber, const Preset & preset);

/**
 * Reads preset from EEPROM.
 * @param presetNumber 0 .. (n_presets - 1)
 */
void readPreset(int presetNumber, Preset & preset) {
  // TODO checksum and detection of corrupt data
  byte *b = (byte*)&preset;
  for (int i = 0; i < sizeof(Preset); i++)
    b[i] = EEPROM.read(PresetsAddress + sizeof(Preset)*presetNumber + i);
  if (preset.magic != magic) 
    defaultPreset(preset);
}

/**
 * Saves preset to EEPROM. To maximize EEPROM lifetime only changed values are actually written. 
 * @param presetNumber 0 .. (n_presets - 1)
 */
void savePreset(int presetNumber, const Preset & preset) {
  byte *b = (byte*)&preset;
  for (int i = 0; i < sizeof(Preset); i++) {
    int address = PresetsAddress + sizeof(Preset)*presetNumber + i;
    byte original = EEPROM.read(address);
    if (original != b[i])
      EEPROM.write(address, b[i]);
  }
}

void defaultSound(Sound & sound) {
  sound.bank = BankA;
  sound.program_number = 0; // Grand Piano
  sound.transpose = MIDI_CONTROLLER_MEAN; // means 0
  sound.volume = MIDI_CONTROLLER_MEAN;
  sound.pan = MIDI_CONTROLLER_MEAN;
  sound.reverb_send = MIDI_CONTROLLER_MEAN; // do not change reverb send level
  sound.effects_send = MIDI_CONTROLLER_MEAN; // do not change effects send level
  sound.mod_wheel_ctrl_no = NoWheel; 
  sound.pitch_wheel_ctrl_no = NoWheel; 
  sound.ext_switch_1_ctrl_no = NoSwitch; 
  sound.ext_switch_2_ctrl_no = NoSwitch; 
  sound.cutoff_frequency = MIDI_CONTROLLER_MEAN;
  sound.resonance = MIDI_CONTROLLER_MEAN;
}

void defaultPreset(Preset & preset) {
  preset.magic = magic; // is a valid data set now
  preset.split_point = invalid; // no split left|right

  defaultSound(preset.foot);
  preset.foot.program_number = invalid; // switches mode

  defaultSound(preset.left);
  preset.left.program_number = 26; // Jazz Guitar
  preset.left.ext_switch_1_ctrl_no = Sustain; 

  defaultSound(preset.right);
  preset.right.mod_wheel_ctrl_no = Modulation; 
  preset.right.pitch_wheel_ctrl_no = Pitch;
  preset.right.ext_switch_1_ctrl_no = Sustain; 
  preset.right.ext_switch_2_ctrl_no = Sostenuto; 
}

/*--------------------------------- state event machine ---------------------------------*/

const int n_global_parameters = 3;

const int n_parameter_sets = 4;

enum ParameterSet {
  CommonSettings, Foot, Left, Right  
};

const int n_common_parameters = 1;

enum CommonParameter {
  SplitParam
};

const int n_sound_parameters = 13;

enum SoundParameter {
  BankParam, ProgNoParam, TransposeParam, VolumeParam, PanParam, 
  ReverbParam, EffectsParam, CutoffParam, ResonanceParam,
  ModAssign, PitchAssign, Switch1Assign, Switch2Assign
};

enum State {
  playingSound, 
  playingPreset,
  selectSound,
  selectPreset,
  editPreset,
  editPresetCommon,
  editPresetSound,
  askSavePreset,
  waitFor2ndTransposeKey,
  editGlobals
};

enum Event {
  enterBtn, exitBtn, modWheel, pitchWheel, noteEvent
};

void displaySound(SD2Bank bank, int program_number);
void displayPreset(const Preset & Preset);
void sendSound(SD2Bank bank, midi::DataByte program_number, midi::Channel channel);
void sendSound(const Sound & sound, midi::Channel channel);
void sendSoundParameter(SoundParameter p, byte value, midi::Channel channel);
void process(Event event, int value);








