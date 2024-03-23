// max. value of a MIDI controller
const byte MIDI_CONTROLLER_MAX = 127;
const byte MIDI_CONTROLLER_MEAN = 64;


/*--------------------------------- persistent settings ---------------------------------*/

// the values in a fresh EEPROM
const byte invalid = 0xff;

/*--------- global settings ---------*/
// start address of global settings storage area in EEPROM
const int GlobalSettingsAddress = 0;

const int n_global_settings = 6;

enum GlobalParameter {
  BassBoostParam, BoostFreqParam, // TODO still SD2 params
  VelocitySlopeParam, VelocityOffsetParam, 
  FilterVelocitySlopeParam, FilterVelocityOffsetParam
};

struct GlobalSettings {
  byte SD2_bass_boost;
  byte SD2_boost_freq;
  byte SD2_velo_slope;
  byte SD2_velo_offset;
  byte SD2_filter_velo_slope;
  byte SD2_filter_velo_offset;
  byte reserved7;
  byte reserved8;
  byte reserved9;
};

GlobalSettings globalSettings;

/**
 * Reads global settings from EEPROM.
 * When the EEPROM still contains 0xff defaults,
 * reasonable initial values will be chosen for the global settings.
 */
void readGlobals() {
  // TODO checksum and detection of corrupt data
  byte *b = (byte*)&globalSettings;
  for (int i = 0; i < sizeof(GlobalSettings); i++)
    b[i] = EEPROM.read(GlobalSettingsAddress + i);
  /*  
  if (globalSettings.SD2_bass_boost > max_boost_gain)
    globalSettings.SD2_bass_boost = 0;
  if (globalSettings.SD2_boost_freq > max_boost_freq)
    globalSettings.SD2_boost_freq = max_boost_freq/2;
  if (globalSettings.SD2_velo_slope > MIDI_CONTROLLER_MAX)
    globalSettings.SD2_velo_slope = MIDI_CONTROLLER_MEAN;
  if (globalSettings.SD2_velo_offset > MIDI_CONTROLLER_MAX)
    globalSettings.SD2_velo_offset = MIDI_CONTROLLER_MEAN;
  if (globalSettings.SD2_filter_velo_slope > MIDI_CONTROLLER_MAX)
    globalSettings.SD2_filter_velo_slope = MIDI_CONTROLLER_MEAN;
  if (globalSettings.SD2_filter_velo_offset > MIDI_CONTROLLER_MAX)
    globalSettings.SD2_filter_velo_offset = MIDI_CONTROLLER_MEAN;
  */  
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
  byte finetune;
  byte volume;
  byte pan;
  byte reverb_type;
  byte reverb_send;
  byte effects_type;
  byte effects_send;
  byte cutoff_frequency;
  byte resonance;
  byte attack_time;
  byte decay_time;
  byte release_time;
  byte vibrato_rate;
  byte vibrato_depth;
  byte vibrato_delay;
  byte reserved1;
  byte reserved2;
  byte reserved3;
  byte mod_wheel_ctrl_no;
  byte pitch_wheel_ctrl_no;
  byte ext_switch_1_ctrl_no;
  byte ext_switch_2_ctrl_no;
  byte channel; // not [yet] adjustable but comfortable here
};

const long magic = 0x20031fe1;

enum PedalMode { BassPedal, ControllerPedal };

/* EEPROM structure */
struct Preset {
public:
  long magic;
  byte split_point;
  byte pedal_mode;
  byte reserved7;
  byte reserved8;
  byte reserved9;
  Sound layer, right, left, foot;
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
  sound.bank = GeneralMIDI;
  sound.program_number = 0; // Grand Piano
  sound.transpose = MIDI_CONTROLLER_MEAN; // means 0
  sound.finetune = MIDI_CONTROLLER_MEAN; // 0 cent
  sound.volume = MIDI_CONTROLLER_MEAN;
  sound.pan = MIDI_CONTROLLER_MEAN;
  sound.reverb_send = MIDI_CONTROLLER_MEAN; // do not change reverb send level
  sound.effects_send = MIDI_CONTROLLER_MEAN; // do not change effects send level
  sound.cutoff_frequency = MIDI_CONTROLLER_MEAN;
  sound.resonance = MIDI_CONTROLLER_MEAN;
  sound.attack_time =  MIDI_CONTROLLER_MEAN;
  sound.decay_time =  MIDI_CONTROLLER_MEAN;
  sound.release_time = MIDI_CONTROLLER_MEAN;
  sound.vibrato_rate = MIDI_CONTROLLER_MEAN;
  sound.vibrato_depth = 0;
  sound.vibrato_delay = MIDI_CONTROLLER_MEAN;
  sound.mod_wheel_ctrl_no = NoWheel; 
  sound.pitch_wheel_ctrl_no = NoWheel; 
  sound.ext_switch_1_ctrl_no = NoSwitch; 
  sound.ext_switch_2_ctrl_no = NoSwitch; 
}

void defaultPreset(Preset & preset) {
  preset.magic = magic; // is a valid data set now
  preset.split_point = invalid; // no split left|right
  preset.pedal_mode = ControllerPedal;

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

  defaultSound(preset.layer); // optional right layer sound
  preset.layer.program_number = 48; // String Ensemble
  preset.layer.volume = 0;
  preset.layer.mod_wheel_ctrl_no = Modulation; 
  preset.layer.pitch_wheel_ctrl_no = Pitch;
  preset.layer.ext_switch_1_ctrl_no = Sustain; 
  preset.layer.ext_switch_2_ctrl_no = Sostenuto; 
}

/*--------------------------------- state event machine ---------------------------------*/

const int n_parameter_sets = 5;

enum ParameterSet {
  CommonParameters, FootParameters, LeftParameters, RightParameters, LayerParameters  
};

const int n_common_parameters = 2;

enum CommonParameter {
  SplitParam,
  PedalModeParam
};

const int n_sound_parameters = 14;

enum SoundParameter {
  BankParam, ProgNoParam, TransposeParam, FinetuneParam, VolumeParam, PanParam, 
  ReverbSendParam, ReverbTypeParam, EffectsSendParam, EffectsTypeParam, 
  CutoffParam, ResonanceParam,
  AttackTimeParam, DecayTimeParam, ReleaseTimeParam, 
  VibratoRateParam, VibratoDepthParam, VibratoDelayParam,
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
  editGlobals,
  showInfo
};

enum Event {
  enterBtn, exitBtn, modWheel, pitchWheel, noteEvent, volumeKnob
};

void displaySound(SoXXLBank bank, int program_number);
void displayPreset(const Preset & Preset);
void sendSound(SoXXLBank bank, midi::DataByte program_number, midi::Channel channel);
void sendSound(const Sound & sound, midi::Channel channel);
void sendSoundParameter(SoundParameter p, byte value, midi::Channel channel);
void process(Event event, int value);
