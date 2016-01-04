// max. value of a MIDI controller
const int MIDI_CONTROLLER_MAX = 127;
const int MIDI_CONTROLLER_MEAN = 64;

/*--------------------------------- LCD display ---------------------------------*/

enum DisplayArea { 
  line1, line2, line1left, line1right, line2left, line2right };

/* display text on the given display area */
void display(DisplayArea area, const char *  text);
void display(DisplayArea area, int value);
void display(DisplayArea area, const char *  text1, const char * text2);
void display(DisplayArea area, const char *  text, int value);
void display(DisplayArea area, int value, const char *  text);

/*--------------------------------- persistent settings ---------------------------------*/

// the values in a fresh EEPROM
const byte invalid = 0xff;

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
  preset.foot.program_number = 32; // Jazz Bass

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
  waitFor2ndTransposeKey
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








