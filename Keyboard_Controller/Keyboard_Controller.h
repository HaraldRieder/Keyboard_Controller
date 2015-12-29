// max. value of a MIDI controller
const int MIDI_CONTROLLER_MAX = 127;
const int MIDI_MEAN_PAN = 0x40;

/*--------------------------------- LCD display ---------------------------------*/

enum DisplayArea { 
  line1, line2, line1left, line1right, line2left, line2right };

/* display text on the given display area */
void display(DisplayArea area, const char *  text);
void display(DisplayArea area, int value);
void display(DisplayArea area, const char *  text1, const char * text2);
void display(DisplayArea area, const char *  text, int value);

/*--------------------------------- persistent settings ---------------------------------*/

// the values in a fresh EEPROM
const byte invalid = -1;

/*--------- main settings ---------*/
// start address of main settings storage area in EEPROM
const int MainSettingsAddress = 0;

struct MainSettings {
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

/*--------- sound presets ---------*/
enum SwitchAssignableControllers {
  NoSwitch = invalid,
  Sustain = midi::Sustain, // values 00 or 0f
  Sostenuto = midi::Sostenuto, // values 00 or 0f
  Soft = midi::SoftPedal, // values 00 or 0f
  WhaWha = 0x54, // values 00 or 0f
  Rotor = 0x1e, // values 00 0ff, 40 slow, 7f fast
};

enum WheelAssignableControllers {
  NoWheel = invalid,
  Modulation = midi::ModulationWheel,
  WhaWhaAmount = 0x55,
  CutoffFrequency = 0x120, // TODO NRPN 
  Resonance = 0x121 // TODO NRPN 
};

const int n_presets = 88; // 1 per key, 50 bytes/Preset * 88 Presets = 4400 bytes EEPROM occupied

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
  byte reserved6;
  byte reserved7;
  byte reserved8;
  byte reserved9;
};

/* EEPROM structure */
struct Preset {
public:
  byte split_point;
  byte reserved6;
  byte reserved7;
  byte reserved8;
  byte reserved9;
  Sound right, left, foot;
};

Preset defaultPreset();
Preset readPreset(int presetNumber);
void savePreset(int presetNumber, Preset preset);

/**
 * Reads preset from EEPROM.
 * @param presetNumber 0 .. (n_presets - 1)
 */
Preset readPreset(int presetNumber) {
  // TODO checksum and detection of corrupt data
  Preset ret;
  byte *b = (byte*)&ret;
  for (int i = 0; i < sizeof(Preset); i++)
    b[i] = EEPROM.read(PresetsAddress + sizeof(Preset)*presetNumber + i);
  if (ret.right.bank == invalid) 
    return defaultPreset();
  return ret;
}

/**
 * Saves preset to EEPROM. To maximize EEPROM lifetime only changed values are actually written. 
 * @param presetNumber 0 .. (n_presets - 1)
 */
void savePreset(int presetNumber, Preset preset) {
  byte *b = (byte*)&preset;
  for (int i = 0; i < sizeof(Preset); i++) {
    int address = PresetsAddress + sizeof(Preset)*presetNumber + i;
    byte original = EEPROM.read(address);
    if (original != b[i])
      EEPROM.write(address, b[i]);
  }
}

Preset defaultPreset() {
  Preset ret;
  ret.split_point = invalid; // no split left|right
  ret.foot.bank = BankA;
  ret.foot.program_number = 32; // Jazz Bass
  ret.foot.transpose = 0;
  ret.foot.volume = MIDI_CONTROLLER_MAX;
  ret.foot.pan = MIDI_MEAN_PAN;
  ret.foot.reverb_send = invalid; // do not change reverb send level
  ret.foot.effects_send = invalid; // do not change effects send level
  ret.foot.mod_wheel_ctrl_no = invalid; // do not send anything
  ret.foot.pitch_wheel_ctrl_no = invalid; // do not send anything
  ret.foot.ext_switch_1_ctrl_no = invalid; // do not send anything
  ret.foot.ext_switch_2_ctrl_no = invalid; // do not send anything
  ret.left.bank = BankA;
  ret.left.program_number = 26; // Jazz Guitar
  ret.left.transpose = 1; // 12 notes higher
  ret.left.volume = MIDI_CONTROLLER_MAX;
  ret.left.pan = MIDI_MEAN_PAN;
  ret.left.reverb_send = invalid; // do not change reverb send level
  ret.left.effects_send = invalid; // do not change effects send level
  ret.left.mod_wheel_ctrl_no = invalid; // do not send anything
  ret.left.pitch_wheel_ctrl_no = invalid; // do not send anything
  ret.left.ext_switch_1_ctrl_no = midi::Sustain; 
  ret.left.ext_switch_2_ctrl_no = invalid; // do not send anything
  ret.right.bank = BankA;
  ret.right.program_number = 0; // Grand Piano
  ret.right.transpose = 0; 
  ret.right.volume = MIDI_CONTROLLER_MAX;
  ret.right.pan = MIDI_MEAN_PAN;
  ret.right.reverb_send = invalid; // do not change reverb send level
  ret.right.effects_send = invalid; // do not change effects send level
  ret.right.mod_wheel_ctrl_no = midi::ModulationWheel; 
  ret.right.pitch_wheel_ctrl_no = ~invalid; // TODO, switches pitch bend messages on
  ret.right.ext_switch_1_ctrl_no = midi::Sustain; 
  ret.right.ext_switch_2_ctrl_no = midi::Sostenuto; 
}

/*--------------------------------- state event machine ---------------------------------*/

enum State {
  playing, // normal state after power on
  selectSound,
  selectPreset
};

enum Event {
  enterBtn, exitBtn, modWheel, pitchWheel, dial
};

void process(Event event);
void process(Event event, int value);








