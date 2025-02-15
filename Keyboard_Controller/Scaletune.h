const int n_scales = 6;

struct Scale
{
	Pitches pitches ;
	const char * name ;	
};

// See also Atari MIDI Enhancer
Scale scales[n_scales] =
{
  {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "equal temperament"},
  {{0, 16, -6, 10, -13, 3, -19, -3, 13, -10, 6, -16}, "meantone"},
//  {{0, -10, 4, -6, 8, -2, 12, 2, -8, 6, -4, 10}, "pythagorean"},
  {{0, 12, 4, 16, -14, -2, 0, 2, 14, -16, -4, -12}, "clean symmetric"},
  {{0, 12, 4, 16, -14, -2, -10, 2, 14, -16, -4, -12}, "clean"},
//  {{0, -30, -18, -96, -14, -2, -10, 2, 84, 6, 18, -12}, "clean 2*2 and 2*9"}, // pitch bend mode only
  {{0, -64, 31, -33, 35, -2, -66, 2, -35, 33, -31, -68}, "sevens (3/2 and 7/6)"},
  {{0, -50, 0, 50, 0, 0, 50, 0, 0, -50, 0, 50}, "arabian"}
};

int current_scale = 0;
byte current_key = invalid;

enum ScaletuneState { tuned, waitForScaletuneKey };

ScaletuneState scaletuneState = tuned;

// within this range scales different from normal can be set
const byte first_note = 23; // an H 
const byte last_note = 12 * n_scales - 1 + first_note;

void displayScale() {
  char b[30];
  display(line1, scales[current_scale].name);
  if (current_scale == 0) {
    // key does not matter
    display(line2, ""); 
  }
  else {
    sprintf(b, "in %i", current_key%12);
    display(line2, b);
  }
}

Pitches current_pitches;

void processScaletune(byte note) {
  if (note < first_note || note > last_note) {
    current_key = invalid;
    current_scale = 0;
  }
  else {
    current_key = note % 12;
    current_scale = (note - first_note) / 12;
  }
  for (int i = 0; i < 12; i++) {
    current_pitches[(note + i) % 12] = scales[current_scale].pitches[i];
  }
  displayScale();
}

