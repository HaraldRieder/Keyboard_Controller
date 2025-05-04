#include "midi_Defs.h"
// max. value of a MIDI controller
const byte MIDI_CONTROLLER_MAX = 127;
const byte MIDI_CONTROLLER_MEAN = 64;


/*--------------------------------- persistent settings ---------------------------------*/

// the values in a fresh EEPROM
const byte invalid = 0xff;

/*--------- global settings ---------*/
// start address of global settings storage area in EEPROM
const int GlobalSettingsAddress = 0;

const int n_global_settings = 1;

enum GlobalParameter {
  VelocityMapParam
};

struct GlobalSettings {
  byte velocity_map;
  byte reserved2;
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
 * When the EEPROM still contains 0xff defaults,
 * reasonable initial values will be chosen for the global settings.
 */
void readGlobals() {
  // TODO checksum and detection of corrupt data
  byte *b = (byte*)&globalSettings;
  for (int i = 0; i < sizeof(GlobalSettings); i++)
    b[i] = EEPROM.read(GlobalSettingsAddress + i);
  if (globalSettings.velocity_map >= MIDI_CONTROLLER_MAX)
    globalSettings.velocity_map = 0;
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
const int n_switch_assignable_ctrls = 5;
enum SwitchController {
  NoSwitchCtrl = invalid,
  Sustain = midi::Sustain, // values 00 or 7f
  Sostenuto = midi::Sostenuto, // values 00 or 7f
  Soft = midi::SoftPedal, // values 00 or 7f
  Portamento = midi::Portamento // values 00 or 7f
};

const int n_continuous_assignable_ctrls = 8;
enum ContinuousController {
  NoContCtrl = invalid,
  Modulation = midi::ModulationWheel,
  CutoffFrequency = 0x4A,
  Resonance = 0x47,
  PortamentoTime = midi::PortamentoTime,
  PortamentoControl = midi::PortamentoControl,
  Expression = midi::ExpressionController,
  InverseExpression = 128 + midi::ExpressionController // emulated, for crossfading
};

const int n_pitch_wheel_assignable_ctrls = 2;
enum PitchWheelController {
  NoPitchCtrl = invalid,
  Pitch = 0xfe
};

const int n_presets = 35; // 116 bytes/preset * 35 presets = 4060 bytes EEPROM occupied by presets
const int n_sounds_per_preset = 4; // foot + left hand + right hand [+ right layered]

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
  byte reserved1;
  byte reverb_send;
  byte effects_send;
  byte reserved2;
  byte cutoff_frequency;
  byte resonance;
  byte attack_time;
  byte decay_time;
  byte release_time;
  byte vibrato_rate;
  byte vibrato_depth;
  byte vibrato_delay;
  byte reserved3;
  byte reserved4;
  byte reserved5;
  byte mod_wheel_ctrl_no;
  byte pitch_wheel_ctrl_no;
  byte ext_switch_1_ctrl_no;
  byte ext_pedal_ctrl_no;
  byte channel; // not [yet] adjustable but comfortable here
};

// change magic value to force init of each preset
const long magic = 0x2e031fe5;

const int n_pedal_modes = 2;
enum PedalMode { BassPedal, ControllerPedal };

/* EEPROM structure */
struct Preset {
public:
  long magic;
  byte split_point;
  byte pedal_mode;
  byte fx1_type;
  byte fx2_type;
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
  sound.bank = Piano;
  sound.program_number = 0; // GM 
  sound.transpose = MIDI_CONTROLLER_MEAN; // means 0
  sound.finetune = MIDI_CONTROLLER_MEAN; // 0 cent
  sound.volume = 100;
  sound.pan = MIDI_CONTROLLER_MEAN;
  sound.reverb_send = 15;
  sound.effects_send = 0;
  sound.cutoff_frequency = MIDI_CONTROLLER_MEAN;
  sound.resonance = 0;
  sound.attack_time =  MIDI_CONTROLLER_MEAN;
  sound.decay_time =  MIDI_CONTROLLER_MEAN;
  sound.release_time = MIDI_CONTROLLER_MEAN;
  sound.vibrato_rate = MIDI_CONTROLLER_MEAN;
  sound.vibrato_depth = 0;
  sound.vibrato_delay = MIDI_CONTROLLER_MEAN;
  sound.mod_wheel_ctrl_no = NoContCtrl; 
  sound.pitch_wheel_ctrl_no = NoPitchCtrl; 
  sound.ext_switch_1_ctrl_no = NoSwitchCtrl; 
  sound.ext_pedal_ctrl_no = NoContCtrl; 
}

void defaultPreset(Preset & preset) {
  preset.magic = magic; // is a valid data set now
  preset.split_point = invalid; // no split left|right
  preset.pedal_mode = ControllerPedal;
  preset.fx1_type = HALL1;
  preset.fx2_type = CHORUS1;

  defaultSound(preset.foot);
  preset.foot.finetune = MIDI_CONTROLLER_MEAN - 2;
  preset.foot.program_number = 32; // Jazz Bass 

  defaultSound(preset.left);
  preset.left.bank = Piano;
  preset.left.program_number = 5; // classic grand piano with resonance
  preset.left.cutoff_frequency = 22;
  preset.left.mod_wheel_ctrl_no = Modulation; 
  preset.left.pitch_wheel_ctrl_no = Pitch;
  preset.left.ext_switch_1_ctrl_no = Sustain; 
  preset.left.ext_pedal_ctrl_no = NoContCtrl; 

  defaultSound(preset.right);
  preset.right.bank = Piano;
  preset.right.program_number = 5; // classic grand piano with resonance
  preset.right.cutoff_frequency = 22;
  preset.right.mod_wheel_ctrl_no = Modulation; 
  preset.right.pitch_wheel_ctrl_no = Pitch;
  preset.right.ext_switch_1_ctrl_no = Sustain; 
  preset.right.ext_pedal_ctrl_no = NoContCtrl; 

  defaultSound(preset.layer); // optional right layer sound
  preset.layer.program_number = 48; // String Ensemble
  preset.layer.volume = 0;
  preset.layer.mod_wheel_ctrl_no = Modulation; 
  preset.layer.pitch_wheel_ctrl_no = Pitch;
  preset.layer.ext_switch_1_ctrl_no = Sustain; 
  preset.layer.ext_pedal_ctrl_no = Expression; 
}

/*--------------------------------- state event machine ---------------------------------*/

const int n_parameter_sets = 5;

enum ParameterSet {
  CommonParameters, FootParameters, LeftParameters, RightParameters, LayerParameters  
};

const int n_common_parameters = 4;

enum CommonParameter {
  SplitParam,
  PedalModeParam,
  FX1TypeParam,
  FX2TypeParam
};

const int n_sound_parameters = 20;

enum SoundParameter {
  BankParam, ProgNoParam, TransposeParam, FinetuneParam, VolumeParam, PanParam, 
  ReverbSendParam, EffectsSendParam, CutoffParam, ResonanceParam,
  AttackTimeParam, DecayTimeParam, ReleaseTimeParam, 
  VibratoRateParam, VibratoDepthParam, VibratoDelayParam,
  ModAssign, PitchAssign, Switch1Assign, /*Switch2Assign,*/ ControlPedalAssign
};

enum State {
  playingSound, 
  playingPreset,
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
void sendGMReset();
void process(Event event, int value);
