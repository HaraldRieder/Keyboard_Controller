#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <MIDI.h>
#include <MemoryFree.h>
#include "SonorityXXL.h"
#include "Pedal.h"
#include "Display.h"
#include "Velocity.h"
#include "Keyboard_Controller.h"
#include "Scaletune.h"

const char * MY_NAME = "DEMIAN";

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, midi3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);

/* data input indicator (MIDI, foot pedal) */
const int led_pin = 13;

/* external switch 1 (damper pedal) pin, last known value and switch type opener/closer */
const int ext_switch_1_pin = 53;
int ext_switch_1_val;
boolean ext_switch_1_opener = false;
/* external switch 2 (pedal) pin, last known value and switch type opener/closer */
//const int ext_switch_2_pin = 51;
//int ext_switch_2_val;
//boolean ext_switch_2_opener = false;
/* external controller pedal, same socket as external switch 2 */
const int external_control_pin = A7; 
const int external_control_maxval = 885; // with 68k popup and 450k controller pedal (log.)
int external_control_val = external_control_maxval; // analog value
int external_val = MIDI_CONTROLLER_MAX; // 0..127 MIDI value
/* enter (red) button pin and last known value */
const int push_btn_enter_pin = 52;
int push_btn_enter_val = HIGH;
/* exit (black) button pin and last known value */
const int push_btn_exit_pin = 50;
int push_btn_exit_val = HIGH;
/* pitch bend wheel input pin and last known value */
const int pitch_wheel_pin = A0;
int pitch_wheel_val;
boolean pitch_wheel_up;
/* modulation wheel input pin and last known value */
const int mod_wheel_pin = A1;
int mod_wheel_val;
boolean mod_wheel_up;
/* volume control */
const int volume_control_pin = A2;
int volume_control_val = 1023; // 0..1023 analog value
int volume_val = MIDI_CONTROLLER_MAX; // 0..127 MIDI value
/* internal switch */
const int int_switch_pin = A3;
boolean int_switch_on;

/* number of current preset, initially the first preset saved in EEPROM */
int preset_number = 0, cancel_number;
Preset currentPreset;
Sound * currentPresetSounds[n_sounds_per_preset] = 
  { &currentPreset.right, &currentPreset.left, &currentPreset.foot, &currentPreset.layer };

const midi::Channel sound_channel = 15;
// right ch. = left ch. + 1, compatible to Matix_to_MIDI project
const midi::Channel right_channel = 2, left_channel = 1, foot_channel = 3, layer_channel = 4; 

/*--------------------------------- setup and main loop ---------------------------------*/

int slice_counter = 0;
const int n_slices = 9;

// settable channels not yet implemented, use the default channels
void readPresetDefaultChannels(int presetNumber, Preset & preset) {
  readPreset(presetNumber, preset);
  preset.left.channel = left_channel;
  preset.right.channel = right_channel;
  preset.foot.channel = foot_channel;
  preset.layer.channel = layer_channel;
}

void setup() {
  Serial.begin(/*9600*/500000); // debugging
  // set up the LCD's number of columns and rows: 
  lcd.begin(lcd_columns, lcd_rows);

  pinMode(led_pin, OUTPUT);
  pinMode(ext_switch_1_pin, INPUT_PULLUP);
  //pinMode(ext_switch_2_pin, INPUT); // ext. 68k pullup, allow 470k logarithmic controller pedal
  pinMode(push_btn_enter_pin, INPUT_PULLUP);
  pinMode(push_btn_exit_pin, INPUT_PULLUP);
  
  setupPedalPins();

  midi::Channel in_channel = 1; // GT-2 mini channel
  midi2.setHandleNoteOn(handleNoteOn);
  midi2.setHandleNoteOff(handleNoteOff);
  midi2.begin(in_channel);
  midi2.setThruFilterMode(midi::Thru::Full);
  midi3.begin(MIDI_CHANNEL_OMNI);
  midi3.setThruFilterMode(midi::Thru::Full);

  ext_switch_1_val = digitalRead(ext_switch_1_pin);
  ext_switch_1_opener = (ext_switch_1_val == LOW);
  //ext_switch_2_val = digitalRead(ext_switch_2_pin);
  //ext_switch_2_opener = (ext_switch_2_val == LOW);

  display(line1, MY_NAME);
  char b[30];
  sprintf(b, "%i bytes free", freeMemory());
  display(line2, b);
  delay(2500);
  //display(line1, "Ext.sw.1:", ext_switch_1_opener?"opener":"closer");
  //display(line2, "Ext.sw.2:", ext_switch_2_opener?"opener":"closer");
  display(line1, "External switch:");
  display(line2, ext_switch_1_opener?"opener":"closer");
  delay(2500);
  // V3 XXL needs these 2 * 2.5 seconds to get ready!
    
  readGlobals();
  sendGlobals(VelocityMapParam);
  readPresetDefaultChannels(preset_number, currentPreset);
  sendPreset(currentPreset);
  dumpSettingsToSerial(); // with 500000 baud takes another ~0.6 seconds
  displayPreset(currentPreset, preset_number);
  
  // disable timers / avoid MIDI jitter
  //TIMSK0 = 0; leave timer 0 enabled so that we still have delay() and millis() but not tone()
  TIMSK1 = 0;
  TIMSK2 = 0;
  TIMSK3 = 0;
  TIMSK4 = 0;
  TIMSK5 = 0;
  Serial.println("Demian ready");
}

/* noise supression by hysteresis */
const int wheel_hysteresis = 3;

// max. time Arduino consumes between 2 calls of loop()
int max_ex_loop_time_ms;
int t_start = -1;

void loop() {
  if (t_start > 0)
    max_ex_loop_time_ms = max(max_ex_loop_time_ms, millis() - t_start);
  
  int inval;
  
  // call this often
  midi2.read();
  midi3.read();

  switch (slice_counter) {
    case 0:
      inval = digitalRead(push_btn_enter_pin);
      if (inval != push_btn_enter_val) {
        push_btn_enter_val = inval;
        if (inval == HIGH)
          process(enterBtn, inval);
      }
      break;
    case 1:
      inval = digitalRead(push_btn_exit_pin);
      if (inval != push_btn_exit_val) {
        push_btn_exit_val = inval;
        if (inval == HIGH)
          process(exitBtn, inval);
      }
      break;
    case 2:
      inval = digitalRead(ext_switch_1_pin);
      if (inval != ext_switch_1_val) 
        handleExtSwitch1(ext_switch_1_val = inval);
      break;
    case 3:
      //inval = digitalRead(ext_switch_2_pin);
      //if (inval != ext_switch_2_val)
      //  handleExtSwitch2(ext_switch_2_val = inval);
      //break;
    case 4:
      inval = analogRead(pitch_wheel_pin);
      if (inval > pitch_wheel_val) {
        if (pitch_wheel_up || inval > pitch_wheel_val + wheel_hysteresis) {
          handlePitchWheel(pitch_wheel_val = inval);
          pitch_wheel_up = true;
        }
      } else if (inval < pitch_wheel_val) {
        if (!pitch_wheel_up || inval < pitch_wheel_val - wheel_hysteresis) {
          handlePitchWheel(pitch_wheel_val = inval);
          pitch_wheel_up = false;
        }
      }
      break;
    case 5:
      inval = analogRead(mod_wheel_pin);
      if (inval > mod_wheel_val) {
        if (mod_wheel_up || inval > mod_wheel_val + wheel_hysteresis) {
          handleModWheel(mod_wheel_val = inval);
          mod_wheel_up = true;
        }
      } else if (inval < mod_wheel_val) {
        if (!mod_wheel_up || inval < mod_wheel_val - wheel_hysteresis) {
          handleModWheel(mod_wheel_val = inval);
          mod_wheel_up = false;
        }
      }
      break;
    case 6:
      inval = analogRead(volume_control_pin);
      // if changed, with +-2 jitter suppression
      if (inval > volume_control_val + 2 || inval < volume_control_val - 2) {
        volume_control_val = inval;
        inval /= 8;
        if (inval != volume_val) {
          process(volumeKnob, inval);
        }
      }
      break;
    case 7:
      inval = analogRead(external_control_pin);
      // if changed, with +-2 jitter suppression
      if (inval > external_control_val + 2 || inval < external_control_val - 2) {
        external_control_val = inval;
        // because of the right choice of pullup, values arrive linear enough
        inval = min((long)inval * MIDI_CONTROLLER_MAX / external_control_maxval, MIDI_CONTROLLER_MAX);
        if (inval != external_val) {
          externalControl(external_val = inval);
          //Serial.print("external val "); Serial.println(external_val);
        }
      }
      break;
    case 8:
      int_switch_on = analogRead(int_switch_pin) > 500; // 0..1023 range
      if (int_switch_on) {
        lcd.setCursor(lcd_columns/2 - 1,0);
        lcd.blink();
      } 
      else {
        lcd.noBlink();
      }
      break;
  }
  midi2.read();
  midi3.read();
  scanPedal();
  
  slice_counter++;
  if (slice_counter >= n_slices)
    slice_counter = 0;

  // call this often
  midi2.read();
  midi3.read();
  t_start = millis();
}

/*--------------------------------- state event machine ---------------------------------*/
/*
playingPreset --exit--> playingSound --exit--> systemInfo --> playingPreset ...
systemInfo --enter--> // TODO edit system settings if needed
playingPreset --enter--> editPreset --exit--> playingPreset
*/

State state = playingPreset;
Sound * editedSound = &currentPreset.right;
byte * param_value ;

/* in sound mode */
int program_number = 0;
enum SoXXLBank current_bank = GeneralMIDI;

/**
 * The state event machine for the user interface.
 * @param event user action
 * @value optional value, meaning depends on event type
 */
void process(Event event, int value) {
  static GlobalParameter global_parameter = VelocityMapParam;
  static ParameterSet parameter_set = CommonParameters;
  static CommonParameter common_parameter = SplitParam;
  static SoundParameter sound_parameter = BankParam;
  static int int_param_value;
  //SoXXLMessage msg;
  switch (state) {

    case playingPreset:
      switch (event) {
        case exitBtn:
          state = playingSound;
          sendGMReset();
          sendCommonParameter(FX1TypeParam, ROOM1);
          sendCommonParameter(FX2TypeParam, CHORUS1);
          sendSound(current_bank, program_number, sound_channel);
          sendSoundParameter(VolumeParam, 100, sound_channel);
          sendSoundParameter(TransposeParam, MIDI_CONTROLLER_MEAN, sound_channel);
          sendSoundParameter(PanParam, MIDI_CONTROLLER_MEAN, sound_channel);
          sendSoundParameter(ReverbSendParam, 10, sound_channel);
          displaySound(current_bank, program_number);
          return;
        case enterBtn:
          state = editPreset;
          display(line1, "Edit Preset");
          parameter_set = CommonParameters;
          displayParameterSet(line2, currentPreset, parameter_set);
          return;
        case volumeKnob:
          if (int_switch_on) {
            preset_number = value * n_presets / (MIDI_CONTROLLER_MAX+1);
            readPresetDefaultChannels(preset_number, currentPreset);
            sendPreset(currentPreset);
            displayPreset(currentPreset, preset_number);
          } 
          else {
            sendVolumes(currentPreset, volume_val = value);
          }
          return;
      }
      return;
      
    case playingSound: 
      switch (event) {
        case enterBtn:
        case exitBtn:
          state = editGlobals;
          display(line1, "Global Settings");
          setParamValuePointer(global_parameter);
          int_param_value = map_from_byte(global_parameter, *param_value);
          displayGlobalParameter(global_parameter, *param_value);
          return;
        case volumeKnob:
          if (int_switch_on) {
            program_number = value; 
            sendSound(current_bank, program_number, sound_channel);
            displaySound(current_bank, program_number);
          }
          else {
            sendSoundParameter(VolumeParam, volume_val = value, sound_channel);
          }
          return;
        case modWheel: // internal switch is down
          // bank select, program number remains unchanged
          value = value * n_SoXXL_banks / (MIDI_CONTROLLER_MAX+1);
          SoXXLBank bank = toSoXXLBank(value);
          if (bank != current_bank) {
            current_bank = bank;
            sendSound(current_bank, program_number, sound_channel);
            displaySound(current_bank, program_number);
          }
          return;
      }
      return;
      
    case editGlobals:
      switch (event) {
        case exitBtn:
          saveGlobals();
          state = showInfo;
          displayInfo();
          delay(2000);
          state = playingPreset;
          sendPreset(currentPreset);
          displayPreset(currentPreset, preset_number);
          return;
        case modWheel:
          value = value * n_global_settings / (MIDI_CONTROLLER_MAX+1);
          if (value != global_parameter) {
            global_parameter = (GlobalParameter)value;
            setParamValuePointer(global_parameter);
            int_param_value = map_from_byte(global_parameter, *param_value);
            displayGlobalParameter(global_parameter, *param_value);
          }
          return;
        case pitchWheel:
          // increment/decrement by 1 or by 10 
          {
            int mini, maxi, range;
            getMinMaxRange(global_parameter, mini, maxi, range);
            if (handlePitchWheelEvent(value, mini, maxi, &int_param_value)) {
              *param_value = map_to_byte(global_parameter, int_param_value);
              displayGlobalParameter(global_parameter, *param_value);
              sendGlobals(global_parameter);
            }
          }
          return;
        case volumeKnob:
          if (int_switch_on) {
            int mini, maxi, range;
            getMinMaxRange(global_parameter, mini, maxi, range);
            int_param_value = mini + value * range / (MIDI_CONTROLLER_MAX+1) ;
            *param_value = map_to_byte(global_parameter, int_param_value);
            displayGlobalParameter(global_parameter, *param_value);
            sendGlobals(global_parameter);
          }
          else {
            sendSoundParameter(VolumeParam, volume_val = value, sound_channel);
          }
          return;
      }
      return;

    case showInfo:
      switch (event) {
        case exitBtn:
          state = playingPreset;
          sendPreset(currentPreset);
          displayPreset(currentPreset, preset_number);
          return;
      }
      return;

    case editPreset:
      switch (event) {
        case exitBtn:
          state = askSavePreset;
          if (changed(currentPreset, preset_number)) {
            display(line1, "Save changed preset?");
          }
          else {
            display(line1, "Copy preset?");
          }
          display(line2, "red=yes black=no");
          delay(1500);
          displayDestinationPreset(cancel_number = preset_number);
          return;
        case enterBtn:
          common_parameter = SplitParam;
          sound_parameter = BankParam;
          switch (parameter_set) {
            case CommonParameters:
              state = editPresetCommon;
              setParamValuePointer(common_parameter);
              int_param_value = map_from_byte(common_parameter, *param_value);
              displayCommonParameter(common_parameter, *param_value);
              break;
            case FootParameters:
              state = editPresetSound;
              editedSound = &currentPreset.foot;
              setParamValuePointer(sound_parameter);
              int_param_value = map_from_byte(sound_parameter, *param_value);
              displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
              break;
            case LeftParameters:
              state = editPresetSound;
              editedSound = &currentPreset.left;
              setParamValuePointer(sound_parameter);
              int_param_value = map_from_byte(sound_parameter, *param_value);
              displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
              break;
            case RightParameters:
              state = editPresetSound;
              editedSound = &currentPreset.right;
              setParamValuePointer(sound_parameter);
              int_param_value = map_from_byte(sound_parameter, *param_value);
              displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
              break;
            case LayerParameters:
              state = editPresetSound;
              editedSound = &currentPreset.layer;
              setParamValuePointer(sound_parameter);
              int_param_value = map_from_byte(sound_parameter, *param_value);
              displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
              break;
          }
          displayParameterSet(line1, currentPreset, parameter_set);
          return;
        case modWheel:
           // select parameter set
           {
             value = value * n_parameter_sets / (MIDI_CONTROLLER_MAX+1); // now 0..n_parameter_sets
             // depending on the common parameters we have more or less parameter sets: 
             // common, foot, left, right, layer
             // common, left, right, layer (controller pedal)
             // common, foot, left (no split point)
             // common, left (controller pedal, no split point)
             // => correct value when foot or left are not allowed
             if (currentPreset.split_point == invalid && (ParameterSet)value == RightParameters)
               value = LeftParameters;
             if (currentPreset.pedal_mode == ControllerPedal && (ParameterSet)value == FootParameters)
               value = CommonParameters;
             if (value != parameter_set) {
               parameter_set = (ParameterSet)value;
               displayParameterSet(line2, currentPreset, parameter_set);
             }
           }
          return;
      }
      return; 
      
    case editPresetCommon:
      switch (event) {
        case exitBtn:
          state = editPreset;
          display(line1, "Edit Preset");
          displayParameterSet(line2, currentPreset, parameter_set);
          return;
        case modWheel:
           value = value * n_common_parameters / (MIDI_CONTROLLER_MAX+1);
           if (value != common_parameter) {
             common_parameter = (CommonParameter)value;
             setParamValuePointer(common_parameter);
             int_param_value = map_from_byte(common_parameter, *param_value);
             displayCommonParameter(common_parameter, *param_value);
           }
          return;
        case pitchWheel:
          // split point or pedal mode, increment/decrement by 1 or by 10 
          {
            int min = -1;
            int max = MIDI_CONTROLLER_MAX;
            switch (common_parameter) {
              case PedalModeParam: min = 0; max = n_pedal_modes - 1; break;
              case FX1TypeParam: min = 0; max = n_SoXXL_reverbs - 1; break;
              case FX2TypeParam: min = 0; max = n_SoXXL_effects - 1; break;
            }
            if (handlePitchWheelEvent(value, min, max, &int_param_value)) {
              *param_value = map_to_byte(common_parameter, int_param_value);
              displayCommonParameter(common_parameter, *param_value);
              sendCommonParameter(common_parameter, *param_value);
            }
          }
          return;
        case volumeKnob:
          if (int_switch_on) {
            int min = -1;
            int max = MIDI_CONTROLLER_MAX;
            switch (common_parameter) {
              case PedalModeParam: min = 0; max = n_pedal_modes - 1; break;
              case FX1TypeParam: min = 0; max = n_SoXXL_reverbs - 1; break;
              case FX2TypeParam: min = 0; max = n_SoXXL_effects - 1; break;
            }
            int_param_value = min + value * (max-min+1) / (MIDI_CONTROLLER_MAX+1) ;
            *param_value = map_to_byte(common_parameter, int_param_value);
            displayCommonParameter(common_parameter, *param_value);
            sendCommonParameter(common_parameter, *param_value);
          }
          else {
            sendVolumes(currentPreset, volume_val = value);
          }
          return;
        case noteEvent:
          if (common_parameter == SplitParam) {
            *param_value = (byte)value;
            displayCommonParameter(common_parameter, *param_value);
            delay(1500);
            common_parameter = PedalModeParam;
            setParamValuePointer(common_parameter);
            int_param_value = map_from_byte(common_parameter, *param_value);
            displayCommonParameter(common_parameter, *param_value);
          }
          return;
      }
      return; 
      
    case editPresetSound:
      switch (event) {
        case exitBtn:
          state = editPreset;
          display(line1, "Edit Preset");
          displayParameterSet(line2, currentPreset, parameter_set);
          return;
        case enterBtn:
          return;
        case modWheel:
           value = value * n_sound_parameters / (MIDI_CONTROLLER_MAX+1);
           if (value != sound_parameter) {
             sound_parameter = (SoundParameter)value;
             setParamValuePointer(sound_parameter);
             int_param_value = map_from_byte(sound_parameter, *param_value);
             displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
           }
          return;
        case pitchWheel:
          // increment/decrement by 1 or by 10 
          {
            int mini, maxi, range;
            getMinMaxRange(sound_parameter, mini, maxi, range);
            if (handlePitchWheelEvent(value, mini, maxi, &int_param_value)) {
              *param_value = map_to_byte(sound_parameter, int_param_value);
              displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
              sendPresetSoundParameter(sound_parameter, *param_value);
            }
          }
          return;
        case volumeKnob:
          if (int_switch_on) {
            int mini, maxi, range;
            getMinMaxRange(sound_parameter, mini, maxi, range);
            int_param_value = mini + value * range / (MIDI_CONTROLLER_MAX+1) ;
            *param_value = map_to_byte(sound_parameter, int_param_value);
            displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
            sendPresetSoundParameter(sound_parameter, *param_value);
          }
          else {
            sendVolumes(currentPreset, volume_val = value);
          }
          return;
        case noteEvent:
          if (sound_parameter == TransposeParam) {
            int_param_value = value; // remember
            state = waitFor2ndTransposeKey;
            display(line1, "Please press");
            display(line2, "second key!");
          }
          return;
      }
      return; 
      
    case waitFor2ndTransposeKey:
      switch (event) {
        case noteEvent: 
          *param_value = (byte)(value - int_param_value + MIDI_CONTROLLER_MEAN);
          sendPresetSoundParameter(sound_parameter, *param_value);
        case exitBtn:
          displayParameterSet(line1, currentPreset, parameter_set);
          displaySoundParameter(sound_parameter, *param_value, (SoXXLBank)editedSound->bank);
          state = editPresetSound;
      }
      return;
      
    case askSavePreset:
      switch (event) {
        case enterBtn:
          savePreset(preset_number, currentPreset);
          display(line1, "Preset saved!");
          display(line2, "");
          delay(1500);
          state = playingPreset;
          sendPreset(currentPreset);
          displayPreset(currentPreset, preset_number);
          return;
        case exitBtn:
          state = playingPreset;
          sendPreset(currentPreset);
          displayPreset(currentPreset, preset_number = cancel_number);
          return;
        case pitchWheel:
          // destination preset select
          if (handlePitchWheelEvent(value, 0, n_presets - 1, &preset_number)) {
            displayDestinationPreset(preset_number);
          }
          return;
        case modWheel:
          value = value * n_presets / (MIDI_CONTROLLER_MAX+1);
          if (value != preset_number) {
            preset_number = value;
            displayDestinationPreset(preset_number);
          }
          return;
      }  
  }
}

void displayInfo() {
  display(line1, "Loop delay [ms]:", max_ex_loop_time_ms);
  display(line2, "Pedal scan [ms]:", max_scan_time_ms);
}

const char * toString(GlobalParameter p) {
  switch (p) {
    case VelocityMapParam: return "Velo. Map";
  }
  return "unknown global";
}

/**
 * Display 1 global parameter value in edit mode.
 */ 
void displayGlobalParameter(GlobalParameter p, byte value) {
  display(line2left, toString(p));
  display(line2right, value);
}

/**
 * Display the destination preset number when saving a changed preset.
 */
void displayDestinationPreset(int preset_number) {
  display(line2left, "Save to >>");
  display(line2right, preset_number+1);
}

/**
 * Displays "Sound" in line 1 of the LCD.
 * Displays bank name, program number (starting with 1) and program name in line 2.
 */ 
void displaySound(SoXXLBank bank, int number) {
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
  if (preset.pedal_mode == BassPedal) 
    display(line1right, toString((SoXXLBank)(preset.foot.bank), preset.foot.program_number));
  if (preset.split_point != invalid) {
    display(line2left, toString((SoXXLBank)(preset.left.bank), preset.left.program_number));
    display(line2right, toString((SoXXLBank)(preset.right.bank), preset.right.program_number));
  }
  else if (preset.left.bank != invalid) 
    display(line2, toString((SoXXLBank)(preset.left.bank), preset.left.program_number));
  else 
    display(line2, "Invalid Preset!");
}

/**
 * Sends bank select and program change MIDI messages on channel.
 * @param channel MIDI channel 1..16
 */
void sendSound(SoXXLBank bank, midi::DataByte program_number, midi::Channel channel) {
    midi3.sendControlChange(midi::BankSelect, (midi::DataByte)bank, channel); // only MSB necessary for SD2
    midi3.sendProgramChange(program_number, channel);
    // reset all NRPNs
    midi3.sendControlChange(0x77, 0, channel);
    sendSoundParameter(VolumeParam, volume_val, channel);
}

/**
 * Sends sound settings of 1 preset part to MIDI.
 */ 
void sendSound(const Sound & sound, midi::Channel channel) {
  sendSound((SoXXLBank)sound.bank, sound.program_number, channel);
  sendSoundParameter(TransposeParam, sound.transpose, channel);
  sendSoundParameter(FinetuneParam, sound.finetune, channel);
  sendSoundParameter(VolumeParam, sound.volume, channel);
  sendSoundParameter(PanParam, sound.pan, channel);
  sendSoundParameter(ReverbSendParam, sound.reverb_send, channel);
  sendSoundParameter(EffectsSendParam, sound.effects_send, channel);
  sendSoundParameter(CutoffParam, sound.cutoff_frequency, channel);
  sendSoundParameter(ResonanceParam, sound.resonance, channel);
  sendSoundParameter(AttackTimeParam, sound.attack_time, channel);
  sendSoundParameter(DecayTimeParam, sound.decay_time, channel);
  sendSoundParameter(ReleaseTimeParam, sound.release_time, channel);
  sendSoundParameter(VibratoRateParam, sound.vibrato_rate, channel);
  sendSoundParameter(VibratoDepthParam, sound.vibrato_depth, channel);
  sendSoundParameter(VibratoDelayParam, sound.vibrato_delay, channel);
  sendSoundParameter(PortamentoTimeParam, sound.portamento_time, channel);
}

/**
 * In preset mode common volume parameter controls all channels.
 * @param value 0..127 total volume
 */
void sendVolumes(const Preset & preset, int value) {
  if (preset.pedal_mode == BassPedal) 
    sendSoundParameter(VolumeParam, value*preset.foot.volume/MIDI_CONTROLLER_MAX, foot_channel);
  if (preset.split_point != invalid) {
    sendSoundParameter(VolumeParam, value*preset.right.volume/MIDI_CONTROLLER_MAX, right_channel);
    sendSoundParameter(VolumeParam, value*preset.layer.volume/MIDI_CONTROLLER_MAX, layer_channel);
  }
  sendSoundParameter(VolumeParam, value*preset.left.volume/MIDI_CONTROLLER_MAX, left_channel);   
}

/**
 * Sends all sound settings of the given preset to MIDI.
 */
void sendPreset(const Preset & preset) {
  sendCommonParameter(FX1TypeParam, preset.fx1_type);
  sendCommonParameter(FX2TypeParam, preset.fx2_type);
  if (preset.pedal_mode == BassPedal)
    sendSound(preset.foot, foot_channel); 
  if (preset.split_point != invalid) {
    sendSound(preset.right, right_channel); 
    sendSound(preset.layer, layer_channel);
  }
  sendSound(preset.left, left_channel);
  sendVolumes(preset, volume_val);
}

/**
 * Display name of editable parameter set. 
 * Names of keyboard sets depend on whether there is a split point in the preset.
 */
void displayParameterSet(DisplayArea area, const Preset & preset, const ParameterSet & set) {
  switch (set) {
    case CommonParameters: return display(area, "Common Parameters");
    case FootParameters: return display(area, "Bass Pedal");
    case LeftParameters: return display(area, preset.split_point == invalid ? "Keyboard" : "Left Keyb. Section");
    case RightParameters: return display(area, preset.split_point == invalid ? "Keyboard" : "Right Keyb. Section");
    case LayerParameters: return display(area, preset.split_point == invalid ? "Keyboard" : "Right Keyb. Layer");
  }
}

const char * NONE = "(none)";

void displayCommonParameter(CommonParameter p, byte value) {
  switch (p) {
    case SplitParam: 
      display(line2left, "Split pos:");
      display(line2right, value);
      break;
    case PedalModeParam:
      display(line2left, "Pdl.mode:");
      display(line2right, toString((PedalMode)value));
      break;
    case FX1TypeParam:
      display(line2left, "FX1 type:");
      display(line2right, toString((SoXXLReverbType)value));
      break;
    case FX2TypeParam:
      display(line2left, "FX2 type:");
      display(line2right, toString((SoXXLEffectsType)value));
      break;
  }
  if (value == invalid)
    display(line2right, NONE);
}

byte map_to_byte(GlobalParameter p, int value) {
  return (byte)value;
}

int map_from_byte(GlobalParameter p, byte value) {
  return (int)value;
}

byte map_to_byte(CommonParameter p, int value) {
   switch (p) {
     case PedalModeParam: value = min(1,max(0,value)); break;
     case FX1TypeParam: value =  min(n_SoXXL_reverbs,max(0,value)); break;
     case FX2TypeParam: value =  min(n_SoXXL_effects,max(0,value)); break;
   } 
   return (byte)value;
}

int map_from_byte(CommonParameter p, byte value) {
   return (int)value;
}

byte map_to_byte(SoundParameter p, int value) {
  switch (p) {
    case BankParam:
      return toSoXXLBank(value);
    case ModAssign: 
    case ControlPedalAssign:
      switch (value) {
        case 1: return Modulation;
        case 2: return CutoffFrequency;
        case 3: return Resonance;
        case 4: return Expression;
        case 5: return InverseExpression;
      }
      return NoContCtrl;
    case PitchAssign:
      switch (value) {
        case 1: return Pitch;
        case 2: return PortamentoControl;
      }
      return NoPitchCtrl;
    case Switch1Assign: 
      switch (value) {
        case 1: return Sustain;
        case 2: return Sostenuto;
        case 3: return Soft;
        case 4: return Portamento;
      }
      return NoSwitchCtrl;  
    default:
      return (byte)value;
  }
}

int map_from_byte(SoundParameter p, byte value) {
  switch (p) {
    case BankParam:
      return toIndex((SoXXLBank)value);
    case ModAssign: 
    case ControlPedalAssign:
      switch ((ContinuousController)value) {
        case Modulation: return 1;
        case CutoffFrequency: return 2;
        case Resonance: return 3;
        case Expression: return 4;
        case InverseExpression: return 5;
      }
      return 0;
    case PitchAssign:
      switch ((PitchWheelController)value) {
        case Pitch: return 1;
        case PortamentoControl: return 2;
      }
      return 0;
    case Switch1Assign:
      switch (value) {
        case Sustain: return 1;
        case Sostenuto: return 2;
        case Soft: return 3;
        case Portamento: return 4;
      }
      return 0;
    default:
      return (int)value; // todo sign always ok?
  }
}

const char * toString(ContinuousController c) {
  switch (c) {
    case NoContCtrl: return NONE;
    case Modulation: return "modulation";
    case CutoffFrequency: return "cutoff frq";
    case Resonance: return "resonance"; 
    case Expression: return "expression"; 
    case InverseExpression: return "inv.expres"; 
  }
  return "?";
}

const char * toString(PitchWheelController c) {
  switch (c) {
    case NoPitchCtrl: return NONE;
    case Pitch: return "pitch bend";
    case PortamentoControl: return "portamento";
  }
  return "?";
}

const char * toString(SwitchController c) {
  switch (c) {
    case NoSwitchCtrl: return NONE;
    case Sustain: return "sustain";
    case Sostenuto: return "sostenuto";
    case Soft: return "soft";
    case Portamento: return "portamento";
  }
  return "?";
}

const char * toString(PedalMode m) {
  return m == 0 ? "Bass" : "Controller";
}

const char * toString(SoXXLReverbType t) {
  switch (t) {
    case ROOM1: return "Room 1";
    case ROOM2: return "Room 2";
    case ROOM3: return "Room 3";
    case HALL1: return "Hall 1";
    case HALL2: return "Hall 2";
    case PLATE: return "Plate";
    case DELAY: return "Delay";
    case PAN_DELAY: return "Pan Delay";
  }
  return "?";
}

const char * toString(SoXXLEffectsType t) {
  switch (t) {
    case CHORUS1: return "Chorus 1";
    case CHORUS2: return "Chorus 2";
    case CHORUS3: return "Chorus 3";
    case CHORUS4: return "Chorus 4";
    case FEEDBACK: return "Feedback";
    case FLANGER: return "Flanger";
    case SHORT_DELAY: return "Shrt.Delay";
    case FB_DELAY: return "Fbck.Delay";
  }
  return "?";
}

void displaySoundParameter(SoundParameter p, byte value, SoXXLBank bank) {
  //Serial.print(p);Serial.print(": ");Serial.println(value);
  switch (p) {
    case BankParam: 
      return display(line2, "Sound bank:",toString((SoXXLBank)value));
    case ProgNoParam: 
      return display(line2, "Program:", toString(bank, value));
    case TransposeParam:
      return display(line2, "Transpose:", (int)value - MIDI_CONTROLLER_MEAN);
    case FinetuneParam:
      return display(line2, "Fine tune:", (int)value - MIDI_CONTROLLER_MEAN);
    case VolumeParam:
      return display(line2, "Volume:", value);
    case PanParam:
      display(line2left, "Pan:");
      if (value == MIDI_CONTROLLER_MEAN)
        return display(line2right, "center");
      if (value > MIDI_CONTROLLER_MEAN)
        return display(line2right, ">>>", value - MIDI_CONTROLLER_MEAN);
      return display(line2right, MIDI_CONTROLLER_MEAN - value, "<<<");
    case ReverbSendParam:
      return display(line2, "Reverb send:", value);
    case EffectsSendParam:    
      return display(line2, "FX send:", value);
    case CutoffParam:
      return display(line2, "Cutoff frequ.:", value);
    case ResonanceParam:    
      return display(line2, "Resonance:", value);
    case AttackTimeParam:    
      return display(line2, "Attack:", value);
    case DecayTimeParam:    
      return display(line2, "Decay:", value);
    case ReleaseTimeParam:    
      return display(line2, "Release:", value);
    case VibratoRateParam:    
      return display(line2, "Vibrato rate:", value);
    case VibratoDepthParam:    
      return display(line2, "Vibrato depth:", value);
    case VibratoDelayParam:    
      return display(line2, "Vibrato delay:", value);
    case PortamentoTimeParam:
      return display(line2, "Portamento time:", value);
    case ModAssign:
      return display(line2, "Mod.wheel>", toString((ContinuousController)value));
    case PitchAssign:
      return display(line2, "Ptch.whl.>", toString((PitchWheelController)value));
    case Switch1Assign:
      return display(line2, "Ext.swtch>", toString((SwitchController)value));
    case ControlPedalAssign:
      return display(line2, "Ext.pedal>", toString((ContinuousController)value));
    default:
      return display(line2, "?");
  }
}

/**
 * Sends all global MIDI settings: equalizers, ...
 */
void sendGlobals(GlobalParameter p) {
  SoXXLMessage msg;
  // reset main outputs EQ
  msg = toGlobalNRPNMsg(MainEQLowGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(MainEQLowMidGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(MainEQHighMidGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(MainEQHighGain, MIDI_CONTROLLER_MEAN);
  // reset auxiliary outputs EQ
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(AuxEQLowGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(AuxEQLowMidGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(AuxEQHighMidGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true);
  msg = toGlobalNRPNMsg(AuxEQHighGain, MIDI_CONTROLLER_MEAN);
  midi3.sendSysEx(msg.length, msg.buff, true); 
  switch (p) {
    case VelocityMapParam:
      return init_velocity_map(globalSettings.velocity_map);
  }
}

/**
 * General MIDI Reset
 */
void sendGMReset() {
  SoXXLMessage msg = gmReset();
  midi3.sendSysEx(msg.length, msg.buff, true);
}

void sendSoundParameter(SoundParameter p, byte value, midi::Channel channel) {
  SoXXLMessage msg;
  switch (p) {
    case BankParam: return midi3.sendControlChange(midi::BankSelect, (midi::DataByte)value, channel);
    case ProgNoParam: return midi3.sendProgramChange(value, channel);
    case TransposeParam:
      msg = toNRPNMsg(CoarseTuning, channel - 1, value);
      midi3.sendSysEx(msg.length, msg.buff, true);
      return;
    case FinetuneParam:
      msg = toNRPNMsg(FineTuning, channel - 1, value);
      midi3.sendSysEx(msg.length, msg.buff, true);
      return;
    case VolumeParam: return midi3.sendControlChange(midi::ChannelVolume, (midi::DataByte)value, channel);
    case PanParam: return midi3.sendControlChange(midi::Pan, (midi::DataByte)value, channel);
    case ReverbSendParam: return midi3.sendControlChange(0x5b, value, channel); 
    case EffectsSendParam: return midi3.sendControlChange(0x5d, value, channel);
    case CutoffParam: return  midi3.sendControlChange(midi::SoundController5, value, channel);
    case ResonanceParam: return midi3.sendControlChange(midi::SoundController2, value, channel);   
    case AttackTimeParam: return midi3.sendControlChange(midi::SoundController4, value, channel);
    case DecayTimeParam: return midi3.sendControlChange(midi::SoundController6, value, channel);
    case ReleaseTimeParam: return midi3.sendControlChange(midi::SoundController3, value, channel);
    case VibratoRateParam: return midi3.sendControlChange(midi::SoundController7, value, channel);
    case VibratoDepthParam: return midi3.sendControlChange(midi::SoundController8, value, channel);
    case VibratoDelayParam: return midi3.sendControlChange(midi::SoundController9, value, channel);
    case PortamentoTimeParam: return midi3.sendControlChange(midi::PortamentoTime, value, channel);
  }
}

void sendPresetSoundParameter(SoundParameter p, byte value) {
  midi::Channel ch = left_channel;
  if (editedSound == &currentPreset.right)
    ch = right_channel;
  else if (editedSound == &currentPreset.foot)
    ch = foot_channel;
  else if (editedSound == &currentPreset.layer)
    ch = layer_channel;
  sendSoundParameter(p, *param_value, ch);  
}

void sendCommonParameter(CommonParameter p, byte value) {
  SoXXLMessage msg;
  switch (p) {
    case FX1TypeParam: 
      msg = toReverbTypeMsg(value);
      midi3.sendSysEx(msg.length, msg.buff, true);
      break;
    case FX2TypeParam:
      msg = toEffectTypeMsg(value);
      midi3.sendSysEx(msg.length, msg.buff, true);
      break;
  }
}

void sendScaletune(const Pitches & pitches) {
  // tune each part
  for (byte part = 0; part < 16; part++) {
    SoXXLMessage msg = toScaletuneMsg(part, pitches);
    midi3.sendSysEx(msg.length, msg.buff, true);
  }
}

void setParamValuePointer(GlobalParameter p) {
  switch (p) {
    case VelocityMapParam:
      param_value = &globalSettings.velocity_map;
      break;
  }
}

void setParamValuePointer(CommonParameter p) {
  switch (p) {
    case SplitParam: 
      param_value = &currentPreset.split_point;
      break;
    case PedalModeParam:
      param_value = &currentPreset.pedal_mode;
      break;
    case FX1TypeParam:
      param_value = &currentPreset.fx1_type;
      break;
    case FX2TypeParam:
      param_value = &currentPreset.fx2_type;
      break;
  }
}

void setParamValuePointer(SoundParameter p) {
  switch (p) {
    case BankParam:
      param_value = &(editedSound->bank);
      break;
    case ProgNoParam:
      param_value = &(editedSound->program_number);
      break;
    case TransposeParam:
      param_value = &(editedSound->transpose);
      break;
    case FinetuneParam:
      param_value = &(editedSound->finetune);
      break;
    case VolumeParam:
      param_value = &(editedSound->volume);
      break;
    case PanParam:
      param_value = &(editedSound->pan);
      break;
    case ReverbSendParam:
      param_value = &(editedSound->reverb_send);
      break;
    case EffectsSendParam:
      param_value = &(editedSound->effects_send);
      break;
    case CutoffParam:
      param_value = &(editedSound->cutoff_frequency);
      break;
    case ResonanceParam:
      param_value = &(editedSound->resonance);
      break;
    case AttackTimeParam:
      param_value = &(editedSound->attack_time);
      break;
    case DecayTimeParam:
      param_value = &(editedSound->decay_time);
      break;
    case ReleaseTimeParam:
      param_value = &(editedSound->release_time);
      break;
    case VibratoRateParam:
      param_value = &(editedSound->vibrato_rate);
      break;
    case VibratoDepthParam:
      param_value = &(editedSound->vibrato_depth);
      break;
    case VibratoDelayParam:
      param_value = &(editedSound->vibrato_delay);
      break;
    case PortamentoTimeParam:
      param_value = &(editedSound->portamento_time);
      break;
    case ModAssign:
      param_value = &(editedSound->mod_wheel_ctrl_no);
      break;
    case PitchAssign:
      param_value = &(editedSound->pitch_wheel_ctrl_no);
      break;
    case Switch1Assign:
      param_value = &(editedSound->ext_switch_1_ctrl_no);
      break;
    case ControlPedalAssign:
      param_value = &(editedSound->ext_pedal_ctrl_no);
      break;
  }
}

void getMinMaxRange(GlobalParameter p, int & mini, int & maxi, int & range) {
  mini = 0;
  range = MIDI_CONTROLLER_MAX + 1;
  maxi = mini + range - 1;
}

void getMinMaxRange(SoundParameter p, int & mini, int & maxi, int & range) {
  mini = 0;
  range = MIDI_CONTROLLER_MAX + 1; 
  switch (p) {
    case BankParam:
      range = n_SoXXL_banks;
      break;
    case ModAssign:
    case ControlPedalAssign:
      range = n_continuous_assignable_ctrls;
      break;
    case PitchAssign:
      range = n_pitch_wheel_assignable_ctrls;
      break;
    case Switch1Assign:  
      range = n_switch_assignable_ctrls; 
      break;
  }  
  maxi = mini + range - 1;
}

/**
 * Returns true if given preset differs from preset in EEPROM addressed by the preset number.
 * @preset to be compared
 * @preset_number index of unchanged preset in EEPROM
 * @return true if different
 */
boolean changed(const Preset & preset, int preset_number) {
  Preset originalPreset;
  readPresetDefaultChannels(preset_number, originalPreset);
  return (memcmp(&originalPreset, &preset, sizeof(Preset)) != 0);
}

/**
 * increment/decrement target value by 1 or by 10 (depending on change of pb_value)
 * @param value pitch bend value from MIDI_PITCHBEND_MIN to MIDI_PITCHBEND_MAX
 * @param min_value minimum allowed value for target_value
 * @param max_value maximum allowed value for target_value
 * @param target_value to be incremented/decremented
 * @return true if target_value has been changed
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
  boolean off = ext_switch_1_opener ? (inval == LOW) : (inval == HIGH);
  switch (state) {
    case playingSound:
      midi3.sendControlChange(midi::Sustain, off ? 0 : MIDI_CONTROLLER_MAX, sound_channel);
      break;
    default:
      for (int i = 0; i < n_sounds_per_preset; i++) {
        Sound * s = currentPresetSounds[i];
        if (s->ext_switch_1_ctrl_no != NoSwitchCtrl) {
          midi::DataByte ctrlval = off ? 0 : MIDI_CONTROLLER_MAX; 
          midi3.sendControlChange(s->ext_switch_1_ctrl_no, ctrlval, s->channel);
        }
      }
  }
}

/*--------------------------------- pitch bend wheel ---------------------------------*/

/* values from the A/D converter corresponding to wheel positions */
const unsigned int min_pitch = 376;
const unsigned int min_normal_pitch = 533;
const unsigned int lower_pitch_range = min_normal_pitch - min_pitch;
const unsigned int max_normal_pitch = 554;
const unsigned int max_pitch = 694;
const unsigned int upper_pitch_range = max_pitch - max_normal_pitch;

void handlePitchWheel(unsigned int inval) {
  // force 1 initial MIDI message by initialization with invalid value
  static int pitch = MIDI_PITCHBEND_MAX + 1;

  //display(line1, "pitch", inval);
  unsigned long ulval; // avoids unsigned int overflow
  byte ctrlval; // value for low resolution MIDI controllers
  if (inval >= min_normal_pitch) {
    if (inval <= max_normal_pitch) {
      inval = 0;
    }
    else {
      ulval = MIDI_PITCHBEND_MAX;
      inval = (ulval * (inval - max_normal_pitch)) / upper_pitch_range;
      if (inval > MIDI_PITCHBEND_MAX) {
        inval = MIDI_PITCHBEND_MAX;
      }
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
    ctrlval = ((inval + MIDI_PITCHBEND_MAX + 1) >> 7) & 0x7f;
    //display(line1, "pitch ", inval);
    switch (state) {
      case playingSound:
        midi3.sendPitchBend(pitch, sound_channel);
        break;
      case playingPreset:
        for (int i = 0; i < n_sounds_per_preset; i++) {
          Sound * s = currentPresetSounds[i];
          switch (s->pitch_wheel_ctrl_no) {
            case Pitch:
              midi3.sendPitchBend(pitch, s->channel);
              break;
            case PortamentoControl:
              ctrlval = min(ctrlval, 108); // V3 bug? No sound with values > 108!
              midi3.sendControlChange(midi::PortamentoControl, ctrlval, s->channel);
              break;
            default:
              break;
          }
        }
        break;      
      default:
        process(pitchWheel, pitch);
    }
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
    switch (state) {
      case playingSound:
        if (int_switch_on) {
          process(modWheel, modulation);
        }
        else {
          midi3.sendControlChange(midi::ModulationWheel, modulation, sound_channel);
        }
        break;
      case playingPreset:
        for (int i = 0; i < n_sounds_per_preset; i++) {
          Sound * s = currentPresetSounds[i];
          switch (s->mod_wheel_ctrl_no) {
            case NoContCtrl: 
              break;
            case CutoffFrequency:
              sendSoundParameter(CutoffParam, modulation, s->channel);
              break;
            case Resonance:
              sendSoundParameter(ResonanceParam, modulation, s->channel);
              break;
            case InverseExpression:
              midi3.sendControlChange(midi::ExpressionController, MIDI_CONTROLLER_MAX - modulation, s->channel);
              break;
            default: 
              midi3.sendControlChange(s->mod_wheel_ctrl_no, modulation, s->channel);
          }
        }
        break;
      default:      
        process(modWheel, modulation);
    }
  }
}

/*------------------------------- external controller pedal --------------------------*/

void externalControl(int value) {
  switch (state) {
      case playingSound:
        midi3.sendControlChange(midi::ExpressionController, value, sound_channel);
        break;
      case playingPreset:
        for (int i = 0; i < n_sounds_per_preset; i++) {
          Sound * s = currentPresetSounds[i];
          switch (s->ext_pedal_ctrl_no) {
            case NoContCtrl: 
              break;
            case CutoffFrequency:
              sendSoundParameter(CutoffParam, value, s->channel);
              break;
            case Resonance:
              sendSoundParameter(ResonanceParam, value, s->channel);
              break;
            case InverseExpression:
              midi3.sendControlChange(midi::ExpressionController, MIDI_CONTROLLER_MAX - value, s->channel);
              break;
            default: 
              midi3.sendControlChange(s->ext_pedal_ctrl_no, value, s->channel);
          }
        }
        break;
  }
}

/*--------------------------------- note  on / note off ---------------------------------*/

void handleNoteOn(byte channel, byte note, byte velocity)
{
  //digitalWrite(led_pin, LOW);
  velocity = velocity_map[velocity];
  switch (state) {
    case playingSound: 
      midi3.sendNoteOn(note, velocity, sound_channel);
      break;
    default: // Preset
      if (currentPreset.split_point == invalid)
        midi3.sendNoteOn(note, velocity, left_channel);
      else {
        if (note > currentPreset.split_point) {
          midi3.sendNoteOn(note, velocity, right_channel);
          midi3.sendNoteOn(note, velocity, layer_channel);
        }
        else {
          midi3.sendNoteOn(note, velocity, left_channel);
        }
      }
      // handle note on with velocity 0 like note off
      if (velocity == 0 && state != playingPreset)
        process(noteEvent, note);
  }
  //digitalWrite(led_pin, HIGH);
}

void handleNoteOff(byte channel, byte note, byte velocity)
{
  //digitalWrite(led_pin, LOW);
  if (scaletuneState == waitForScaletuneKey) {
    processScaletune(note);
    sendScaletune(current_pitches);
  }
  switch (state) {
    case playingSound: 
      midi3.sendNoteOff(note, velocity, sound_channel);
      break;
    default: // Preset
      if (currentPreset.split_point == invalid)
        midi3.sendNoteOff(note, velocity, left_channel);
      else {
        if (note > currentPreset.split_point) {
          midi3.sendNoteOff(note, velocity, right_channel);
          midi3.sendNoteOff(note, velocity, layer_channel);
        } 
        else {
          midi3.sendNoteOff(note, velocity, left_channel);
        }
      }
      if (state != playingPreset)
        process(noteEvent, note);
  }
  //digitalWrite(led_pin, HIGH);
}

/*--------------------------------- foot pedal ---------------------------------*/

/* lowest pedal note (without transpose) */
const midi::DataByte F = 29;
const midi::DataByte PedalVelocity = 80;

void handlePedal(int pedal, boolean on) {
  boolean preset_mode = true;
  switch (state) {
    case playingSound:
//    case editGlobals:
    case showInfo:
      preset_mode = false;
      break;
  }
  //digitalWrite(led_pin, LOW);
  if (preset_mode && currentPreset.pedal_mode == BassPedal) {
    midi::DataByte note = (midi::DataByte)(pedal + F);
    if (on) 
      midi3.sendNoteOn(note, PedalVelocity, foot_channel);
    else 
      midi3.sendNoteOff(note, PedalVelocity, foot_channel);
  }
  else {
    // MIDI Controller Pedal
    int controller = -1;
    int change_preset_or_sound_by = 0;
    byte low_value = 0;
    const char * right_text = "";
    switch (pedal) {
    case 0:
      // soft pedal on/off
      controller = midi::SoftPedal;
      right_text = "Soft";
      break;
    case 1:
      change_preset_or_sound_by = -10;
      break;
    case 2:
      // reserved
      return;
    case 3:
      change_preset_or_sound_by = -1;
      break;
    case 4:
      // sustain pedal on/off => left split area
      controller = midi::Sustain;
      right_text = "left Sust.";
      break;
    case 5:
      change_preset_or_sound_by = +1;
      break;
    case 6:
      // sostenuto pedal on/off => left split area
      controller = midi::Sostenuto;
      right_text = "left Sost.";
      break;
    case 7:
      change_preset_or_sound_by = +10;
      break;
    case 8:
      // sostenuto pedal on/off => right split area
      controller = midi::Sostenuto;
      right_text = "Sostenuto";
      break;
    case 9:
      // in sound mode cycle banks down
      if (state == playingSound && on) {
        current_bank = toSoXXLBank(toIndex(current_bank) - 1);
        sendSound(current_bank, program_number, sound_channel);
        displaySound(current_bank, program_number);
      }
      break;
    case 10:
      // sustain pedal on/off => right split area
      controller = midi::Sustain;
      right_text = "Sustain";
      break;
    case 11:
      // in sound mode cycle banks up
      if (state == playingSound && on) {
        current_bank = toSoXXLBank(toIndex(current_bank) + 1);
        sendSound(current_bank, program_number, sound_channel);
        displaySound(current_bank, program_number);
      }
      break;
    case 12:
      // portamento on/off
      controller = midi::Portamento;
      right_text = "Portam.";
      break;
    case 13:
      if (on) {
        if (state == playingSound || state == playingPreset) {
          // scale tune with key select
          scaletuneState = waitForScaletuneKey;
          displayScale();
        }
      }
      else {
        scaletuneState = tuned;
        if (state == playingSound) {
          displaySound(current_bank, program_number);
        }
        else if (state == playingPreset) {
          displayPreset(currentPreset, preset_number);
        }
      }
      return;
    case 14:
      // rotor fast/slow (not off)
      //controller = Rotor;
      //right_text = on ? "Rot. fast":"Rot. slow";
      //low_value = 0x40;
      //break;
      return;
    }
    if (controller > 0) {
      switch (state) {
        case playingSound:
          midi3.sendControlChange(controller, on ? MIDI_CONTROLLER_MAX : low_value, sound_channel);
          break;
        default:
          if (currentPreset.split_point == invalid)
            midi3.sendControlChange(controller, on ? MIDI_CONTROLLER_MAX : low_value, left_channel);
          else {
            // depending on the controller type send to left of right channel or both
            switch (controller) {
              case midi::Sustain:
              case midi::Sostenuto:
                if (right_text[0] != 'l')
                  break; // not meant for left area
              case midi::SoftPedal:
                midi3.sendControlChange(controller, on ? MIDI_CONTROLLER_MAX : low_value, left_channel);
            }
            switch (controller) {
              case midi::Sustain:
              case midi::Sostenuto:
                if (right_text[0] == 'l')
                  break; // not meant for right area
              case midi::SoftPedal:
              case midi::Portamento:
                midi3.sendControlChange(controller, on ? MIDI_CONTROLLER_MAX : low_value, right_channel);
            }
          }
      }
    }
    else if (change_preset_or_sound_by != 0) {
      const char * sign = change_preset_or_sound_by > 0 ? "+" : "-";
      switch (state) {
        case playingSound:
        case playingPreset: 
          if (on) {
            // display the increment/decrement value
            display(line1right, sign, abs(change_preset_or_sound_by));
            return;
          } 
          else {
            // display the new current sound / preset
            if (state == playingPreset) {
              preset_number = max(0, min(n_presets - 1, preset_number + change_preset_or_sound_by));
              readPresetDefaultChannels(preset_number, currentPreset);
              sendPreset(currentPreset);
              displayPreset(currentPreset, preset_number);
            } 
            else {
              program_number = max(0, min(MIDI_CONTROLLER_MAX, program_number + change_preset_or_sound_by));
              sendSound(current_bank, program_number, sound_channel);
              displaySound(current_bank, program_number);
            }
          }
      }
      return;
    } 
    if (currentPreset.pedal_mode != BassPedal) {
      switch (state) {
          case playingSound:
          case playingPreset:
            display(line1right, on ? right_text : "");
      }
    }
  }
  //digitalWrite(led_pin, HIGH);
}

/*-----------------------------------*/

void dumpSoundToSerial(Sound & s) {
  Serial.print(toString((SoXXLBank)s.bank)); Serial.print(" / ");
  Serial.println(toString((SoXXLBank)s.bank,s.program_number));
  Serial.print("    Transpose: "); Serial.print(s.transpose-MIDI_CONTROLLER_MEAN);
  Serial.print(", Fine tune: "); Serial.print(s.finetune-MIDI_CONTROLLER_MEAN);
  Serial.print(", Volume: "); Serial.print(s.volume);
  Serial.print(", Stereo pan: "); Serial.print(s.pan);
  Serial.print(", Reverb send: "); Serial.print(s.reverb_send);
  Serial.print(", Effects send: "); Serial.println(s.effects_send);
  Serial.print("    Cutoff frequency: "); Serial.print(s.cutoff_frequency);
  Serial.print(", Resonance: "); Serial.print(s.resonance);
  Serial.print(", Attack: "); Serial.print(s.attack_time);
  Serial.print(", Decay: "); Serial.print(s.decay_time);
  Serial.print(", Release: "); Serial.println(s.release_time);
  Serial.print("    Vibrato rate: "); Serial.print(s.vibrato_rate);
  Serial.print(", Depth: "); Serial.print(s.vibrato_depth);
  Serial.print(", Delay: "); Serial.print(s.vibrato_delay);
  Serial.print(", Portamento time: "); Serial.println(s.portamento_time);
  Serial.print("    Modulation wheel: "); Serial.print(toString((ContinuousController)s.mod_wheel_ctrl_no));
  Serial.print(", Pitch wheel: "); Serial.print(toString((PitchWheelController)s.pitch_wheel_ctrl_no));
  Serial.print(", External switch: "); Serial.print(toString((SwitchController)s.ext_switch_1_ctrl_no));
  Serial.print(", External pedal: "); Serial.println(toString((ContinuousController)s.ext_pedal_ctrl_no));
}

void dumpSettingsToSerial() {
  Serial.println("Global_Settings");
  Serial.print(" Velocity map: "); Serial.println(globalSettings.velocity_map);
  Serial.println("Presets");
  for (int i = 0; i < n_presets; i++) {
    Preset p;
    readPreset(i,p);
    Serial.print(i+1);
    Serial.print(" Reverb type: "); Serial.print(toString((SoXXLReverbType)p.fx1_type));
    Serial.print(", Effect type: "); Serial.println(toString((SoXXLEffectsType)p.fx2_type));
    if (p.pedal_mode==BassPedal) {
      Serial.print("  Foot Pedal:   "); dumpSoundToSerial(p.foot);
    }
    if (p.split_point == invalid) {
      Serial.print("  Keyboard:   "); dumpSoundToSerial(p.left);
    } else {
      Serial.print("  Keyboard Left:   "); dumpSoundToSerial(p.left);
      Serial.print("  Keyboard Right:   "); dumpSoundToSerial(p.right);
      if (p.layer.volume > 0) {
        Serial.print("  Keyboard Right Layer:   "); dumpSoundToSerial(p.layer);
      }
    }
  }
}