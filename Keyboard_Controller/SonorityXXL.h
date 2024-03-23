/*--------------------------------- V3 Sound Sonority XXL module ---------------------------------*/

/* Ketron SD-2 voice banks with control change values */
enum SoXXLBank {
  GeneralMIDI = 0,
  Piano = 1,
  EPiano = 2,
  Organ = 3,
  Drawbar = 4,
  PercTuned = 5,
  StringInstr = 6,
  Guitar = 7,
  Accordion = 8,
  Strings = 9,
  StringsSolo = 10,
  StringsSynth = 11,
  BrassSolo = 12,
  BrassSection = 13,
  BrassClassic = 14,
  Saxophon = 15,
  Winds = 16,
  WindsClassic = 17,
  Choir = 18,
  Bass = 19,
  Synth = 20,
  FXnPerc = 21
};

enum SoXXLReverbType {
  HALL1 = 0,
  HALL2 = 1,
  ROOM1 = 2,
  ROOM2 = 3,
  ROOM3 = 4,
  PLATE1 = 5,
  PLATE2 = 6,
  PLATE3 = 7,
  CHORUS = 8,
  FLANGE = 9,
  DELAY1 = 10,
  DELAY2 = 11,
  CHORUS_ROOM1 = 12,
  CHORUS_ROOM2 = 13,
  VOCCANCEL = 14,
  ROTARY_SPEAKER = 15
};

enum SoXXLEffectsType {
  CHORUS1 = 0,
  CHORUS2 = 1,
  CHORUS3 = 2,
  CHORUS4 = 3,
  FEEDBACK = 4,
  FLANGER = 5,
  SHORT_DELAY = 6,
  FB_DELAY = 7,
};

const int n_SoXXL_banks = 5;
const int n_SoXXL_parts = 32; // General MIDI parts

/**
 * Returns the name of the sound bank as string with a max. length of 5 characters.
 */
const char * toString(SoXXLBank bank) {
  switch (bank) {
    case GeneralMIDI: return "GM";
    case Piano: return "Piano";
    case EPiano: return "E-Pno";
    case Organ: return "Organ";
    case Drawbar: return "Drawb";
    case PercTuned: return "Percn";
    case StringInstr: return "Harps";
    case Guitar: return "Guitr";
    case Accordion: return "Accrd";
    case Strings: return "String";
    case StringsSolo: return "SSolo";
    case StringsSynth: return "SSynt";
    case BrassSolo: return "Brass";
    case BrassSection: return "BSect";
    case BrassClassic: return "BClas";
    case Saxophon: return "Sax";
    case Winds: return "Winds";
    case WindsClassic: return "WClas";
    case Choir: return "Choir";
    case Bass: return "Bass";
    case Synth: return "Synth";
    case FXnPerc: return "FXPrc";
  }
}

SoXXLBank toSoXXLBank(int index) {
  switch (index) {
    case 1: return Piano;
    case 2: return EPiano;
    case 3: return Organ;
    case 4: return Drawbar;
    case 5: return PercTuned;
    case 6: return StringInstr;
    case 7: return Guitar;
    case 8: return Accordion;
    case 9: return Strings;
    case 10: return StringsSolo;
    case 11: return StringsSynth;
    case 12: return BrassSolo;
    case 13: return BrassSection;
    case 14: return BrassClassic;
    case 15: return Saxophon;
    case 16: return Winds;
    case 17: return WindsClassic;
    case 18: return Choir;
    case 19: return Bass;
    case 20: return Synth;
    case 21: return FXnPerc;
  }
  return GeneralMIDI;
}

int toIndex(SoXXLBank bank) {
  switch (bank) {
    case Piano: return 1;
    case EPiano: return 2;
    case Organ: return 3;
    case Drawbar: return 4;
    case PercTuned: return 5;
    case StringInstr: return 6;
    case Guitar: return 7;
    case Accordion: return 8;
    case Strings: return 9;
    case StringsSolo: return 10;
    case StringsSynth: return 11;
    case BrassSolo: return 12;
    case BrassSection: return 13;
    case BrassClassic: return 14;
    case Saxophon: return 15;
    case Winds: return 16;
    case WindsClassic: return 17;
    case Choir: return 18;
    case Bass: return 19;
    case Synth: return 20;
    case FXnPerc: return 21;
  }
  return 0;
}

const int MAX_NAME_LEN = 14;

// [General Midi]
PROGMEM const char Bank0Names[][MAX_NAME_LEN+1] = {
/*0*/ "Grand Piano",
/*1*/ "Bright Piano",
/*2*/ "E-Grand Piano",
/*3*/ "Honky Tonk",
/*4*/ "E-Piano Tines",
/*5*/ "E-Piano FM",
/*6*/ "Harpsichord",
/*7*/ "Clavinet",
/*8*/ "Celesta",
/*9*/ "Glockenspiel",
/*10*/ "Music Box",
/*11*/ "Vibraphon",
/*12*/ "Marimba",
/*13*/ "Xylophon",
/*14*/ "Tubular Bell",
/*15*/ "Dulcimer",
/*16*/ "Drawbar Organ",
/*17*/ "Percuss. Organ",
/*18*/ "Rock Organ",
/*19*/ "Church Organ",
/*20*/ "Reed Organ",
/*21*/ "Accordion",
/*22*/ "Harmonica",
/*23*/ "Accord. Tango",
/*24*/ "Nylon Guitar",
/*25*/ "Acoustic Steel",
/*26*/ "Jazz Guitar",
/*27*/ "Guitar clean",
/*28*/ "Gtr mute clean",
/*29*/ "Gtr Overdrive",
/*30*/ "Gtr Distortion",
/*31*/ "Gtr harmonics",
/*32*/ "Jazz Bass",
/*33*/ "E-Bass fingerd",
/*34*/ "E-Bass picked",
/*35*/ "Fretless Bass",
/*36*/ "Slap Bass 1",
/*37*/ "Slap Bass 2",
/*38*/ "Synth Bass 1",
/*39*/ "Synth Bass 2",
/*40*/ "Violin",
/*41*/ "Viola",
/*42*/ "Cello",
/*43*/ "Contrabass",
/*44*/ "Tremolo Str",
/*45*/ "Pizzicato Str",
/*46*/ "Harp",
/*47*/ "Timpani",
/*48*/ "Str Ensemble 1",
/*49*/ "Str Ensemble 2",
/*50*/ "Synth Str 1",
/*51*/ "Synth Str 2",
/*52*/ "Choir Aahs",
/*53*/ "Voice Dohs",
/*54*/ "Synth Voice",
/*55*/ "Orchestra Hit",
/*56*/ "Trumpet",
/*57*/ "Trombone",
/*58*/ "Tuba",
/*59*/ "Trp. Mute",
/*60*/ "Horn",
/*61*/ "Brass Section",
/*62*/ "Synth Brass 1",
/*63*/ "Synth Brass 2",
/*64*/ "Sopran Sax",
/*65*/ "Alto Sax",
/*66*/ "Tenor Sax",
/*67*/ "Bariton Sax",
/*68*/ "Oboe",
/*69*/ "English Horn",
/*70*/ "Bassoon",
/*71*/ "Clarinet",
/*72*/ "Piccolo",
/*73*/ "Flute",
/*74*/ "Recorder",
/*75*/ "Pan Flute",
/*76*/ "Blown Bottle",
/*77*/ "Shakuhachi",
/*78*/ "Whistle",
/*79*/ "Ocarina",
/*80*/ "Square Lead",
/*81*/ "Saw Lead",
/*82*/ "Calliope",
/*83*/ "Chiffer Lead",
/*84*/ "Charang",
/*85*/ "Solo Vox",
/*86*/ "5th Saw wave",
/*87*/ "Bass & Lead",
/*88*/ "New Age",
/*89*/ "Warm Pad",
/*90*/ "Polysynth",
/*91*/ "Choir pad",
/*92*/ "Bowed",
/*93*/ "Metallic",
/*94*/ "Halo pad",
/*95*/ "Sweep",
/*96*/ "Rain",
/*97*/ "Soundtrack 5th",
/*98*/ "Crystal",
/*99*/ "Atmosphere",
/*100*/ "Brightness",
/*101*/ "Goblins",
/*102*/ "Echoes",
/*103*/ "Sci-fi Star",
/*104*/ "Sitar",
/*105*/ "Banjo",
/*106*/ "Shamisen",
/*107*/ "Koto",
/*108*/ "Kalimba",
/*109*/ "Bag pipe",
/*110*/ "Fiddle",
/*111*/ "Shanai",
/*112*/ "Tinkle Bell",
/*113*/ "Agogo",
/*114*/ "Steel Drums",
/*115*/ "Woodblock",
/*116*/ "Taiko Drum",
/*117*/ "Melodic Tom",
/*118*/ "Synth Drum",
/*119*/ "Reverse Cymbal",
/*120*/ "Guitar Noise",
/*121*/ "Breath Noise",
/*122*/ "Seashore",
/*123*/ "Bird",
/*124*/ "Telephon",
/*125*/ "Helicopter",
/*126*/ "Applaus",
/*127*/ "Gunshot"
};
// [Piano]
PROGMEM const char Bank1Names[][MAX_NAME_LEN+1] = {
/*0*/ "Grand Piano",
/*1*/ "Grand Pno Res",
/*2*/ "Rock Piano",
/*3*/ "Rock Pno Res",
/*4*/ "Classic Grand",
/*5*/ "Class Gnd Res",
/*6*/ "Honky Tonk",
/*7*/ "Latin Octave",
/*8*/ "Upright Piano",
/*9*/ "Grand + FM-Pno",
/*10*/ "Grand + E-Pno",
/*11*/ "Grand + Pad",
/*12*/ "Grand + String","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [E-Piano]
PROGMEM const char Bank2Names[][MAX_NAME_LEN+1] = {
/*0*/ "Electric Grand",
/*1*/ "Electric Grand",
/*2*/ "Electric Grand",
/*3*/ "Electric Grand",
/*4*/ "E-Piano MK1 Classic",
/*5*/ "E-Piano MK1 Classic",
/*6*/ "E-Piano MK1 Classic",
/*7*/ "E-Piano MK1 Classic",
/*8*/ "E-Piano MK1 Classic",
/*9*/ "E-Piano MK1 Classic",
/*10*/ "E-Piano MK1 Classic",
/*11*/ "E-Piano V3 Bella",
/*12*/ "E-Piano V3 Bella",
/*13*/ "E-Piano V3 Bella",
/*14*/ "E-Piano V3 Bella",
/*15*/ "E-Piano Wurl. A200",
/*16*/ "E-Piano Wurl. A200",
/*17*/ "E-Piano Wurl. A200",
/*18*/ "E-Piano DX Classic",
/*19*/ "E-Piano FM",
/*20*/ "E-Piano FM",
/*21*/ "E-Piano FM",
/*22*/ "E-Piano FM",
/*23*/ "E-Piano FM",
/*24*/ "E-Piano FM",
/*25*/ "E-Piano FM",
/*26*/ "E-Piano FM",
/*27*/ "E-Piano FM","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Organ]
PROGMEM const char Bank3Names[][MAX_NAME_LEN+1] = {
/*0*/ "776555678 slow",
/*1*/ "776555678 fast",
/*2*/ "8_____568 slow",
/*3*/ "8_____568 fast",
/*4*/ "__853____ slow",
/*5*/ "__853____ fast",
/*6*/ "8________ slow",
/*7*/ "8________ fast",
/*8*/ "8_78_____ slow",
/*9*/ "8_78_____ fast",
/*10*/ "8_47_8___ slow",
/*11*/ "8_47_8___ fast",
/*12*/ "8____8___ slow",
/*13*/ "8____8___ fast",
/*14*/ "8_______8 slow",
/*15*/ "8_______8 fast",
/*16*/ "6876_____ slow",
/*17*/ "6876_____ fast",
/*18*/ "888 perc slow",
/*19*/ "Rock Organ",
/*20*/ "Ham Full",
/*21*/ "Ham L100 KW1",
/*22*/ "Ham L100 KW2",
/*23*/ "German DB slow",
/*24*/ "German DB fast",
/*25*/ "German FL slow",
/*26*/ "German FL fast",
/*27*/ "Version UK",
/*28*/ "Theatre Mighty",
/*29*/ "Theatre Xyloph",
/*30*/ "Theatre Bells",
/*31*/ "Theatre Pistn2",
/*32*/ "Theatre P2 Xyl",
/*33*/ "Theatre P2 Bel",
/*34*/ "Theatre Royal",
/*35*/ "Theatre Ro.Xyl",
/*36*/ "Theatre Ro.Bel",
/*37*/ "Theatre Bass",
/*38*/ "Organ House",
/*39*/ "Combo Vibrato1",
/*40*/ "Combo Vibrato2",
/*41*/ "Classic Tutti1",
/*42*/ "Classic Tutti2",
/*43*/ "Classic Flute",
/*44*/ "Classic Combi",
/*45*/ "Classic Flut16",
"-> 118","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","<- 45",
/*118*/ "Theat. Bel Hit",
/*119*/ "Theat. BelShrt",
/*120*/ "Theat. BelReIt",
/*121*/ "Theat. Xyl Hit",
/*122*/ "Theat. XylReit",
/*123*/ "Theat. XyHitRe",
/*124*/ "T. Bell ReIter",
/*125*/ "T. Xylo ReIter",
/*126*/ "Organ Click",
/*127*/ ""
};
// [Organ Drawbar]
PROGMEM const char Bank4Names[][MAX_NAME_LEN+1] = {
/*0*/ "JimmySmith 888",
/*1*/ "JoeyDeFr 888+1",
/*2*/ "Charles Earlnd",
/*3*/ "Brian Auger",
/*4*/ "Garner-Set",
/*5*/ "Piano Set",
/*6*/ "WalterWnd 8__7",
/*7*/ "Whistle",
/*8*/ "Gospel Set",
/*9*/ "Blues Set",
/*10*/ "Easy Listening",
/*11*/ "JSmith 88_ Pc3",
/*12*/ "JSmith 8_8 Pc3",
/*13*/ "Joey Defrancesco",
/*14*/ "Ballad 2",
/*15*/ "Jesse Crawford",
/*16*/ "JoeyDeFran 888",
/*17*/ "Booker T.Jones",
/*18*/ "Green Onions",
/*19*/ "Matthew Fisher",
/*20*/ "Jimmy McGriff",
/*21*/ "Chords __437",
/*22*/ "Chords __84_",
/*23*/ "WalterW __88+6",
/*24*/ "WalterW __8__8",
/*25*/ "LennyD 888__8",
/*26*/ "LnyD 8_8_8_8_8",
/*27*/ "LnyD _88___888",
/*28*/ "Ethel Smith",
/*29*/ "Ken Griffin",
/*30*/ "Jon Lord",
/*31*/ "JSmith 88_ Pc2",
/*32*/ "JSmith 8_8 Pc2",
/*33*/ "Excl.8_8______",
/*34*/ "Excl.838______",
/*35*/ "Std. 888___766",
/*36*/ "Std. 888555666",
/*37*/ "Std. 888888___",
/*38*/ "Std. 888544888",
/*39*/ "Std. 8__888766",
/*40*/ "Xpr. 888006000",
/*41*/ "Xpr. 870060500",
/*42*/ "Xpr. 888850000",
/*43*/ "Xpr. 870650000",
/*44*/ "Std. 870607000",
/*45*/ "Bars 1st Three",
/*46*/ "Bars 1st Four",
/*47*/ "Bar 16",
/*48*/ "Bars 16 - 8",
/*49*/ "Bar 8",
/*50*/ "Bar 5 1/3",
/*51*/ "Vib A (HPF)",
/*52*/ "Vib B (HPF)",
/*53*/ "Full (HPF)",
/*54*/ "Highest (HPF)",
/*55*/ "Mid Mix (HPF)",
/*56*/ "Bars 1st Three",
/*57*/ "Bars 1st Four",
/*58*/ "Bar 16",
/*59*/ "Bar 16 & 8",
/*60*/ "Bar 8",
/*61*/ "Bar 5 1/3",
/*62*/ "Bar 4",
/*63*/ "Bar 2 2/3",
/*64*/ "Bar 2",
/*65*/ "Bar 1 3/5",
/*66*/ "Bar 1 1/3",
/*67*/ "Bar 1",
/*68*/ "Percus. 2",
/*69*/ "Percus. 2 long",
/*70*/ "Percus. 3",
/*71*/ "Percus. 3 long",
/*72*/ "Organ Click","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Perc. Tuned Instruments]
PROGMEM const char Bank5Names[][MAX_NAME_LEN+1] = {
/*0*/ "Music Box",
/*1*/ "MBox oct.delay",
/*2*/ "MBox oct.d.sft",
/*3*/ "Vibraphone",
/*4*/ "Vib. short rel",
/*5*/ "Vib. soft attc",
/*6*/ "Vib. no vibrat",
/*7*/ "Vib. no v.shrt",
/*8*/ "Vib. fast trem",
/*9*/ "Celeste",
/*10*/ "Tinkle",
/*11*/ "Marimba",
/*12*/ "Mar. oct.delay",
/*13*/ "Mar. Xylophoph",
/*14*/ "Xylophone",
/*15*/ "Xylo oct.delay",
/*16*/ "Tubular Bell",
/*17*/ "Tubular Bell 2",
/*18*/ "Timpani",
/*19*/ "Kalimba",
/*20*/ "Cortales",
/*21*/ "Steel drums",
/*22*/ "Alpin Bls Hit",
/*23*/ "Alpin Bls Roll",
/*24*/ "Alpin Bls HiRl",
/*25*/ "Bells","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [String Instruments]
PROGMEM const char Bank6Names[][MAX_NAME_LEN+1] = {
/*0*/ "Harpsichord",
/*1*/ "Harpsi Octave",
/*2*/ "Clavinet 1",
/*3*/ "Clavinet 2",
/*4*/ "Zither",
/*5*/ "Dulcimer 3 str",
/*6*/ "Dulcim.3 str+",
/*7*/ "D.3 str tremol",
/*8*/ "D.3 str bowed",
/*9*/ "Dulcimer 5 str",
/*10*/ "Dulcim.5 str+",
/*11*/ "D.5 str tremol",
/*12*/ "D.5 str bowed",
/*13*/ "Harp",
/*14*/ "Harp long","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
}; 
// [Guitar]
PROGMEM const char Bank7Names[][MAX_NAME_LEN+1] = {
/*0*/ "Git Nylon",
/*1*/ "Git Nylon soft",
/*2*/ "Git Nylon octv",
/*3*/ "Git Nylon slid",
/*4*/ "Git Nylon harm",
/*5*/ "Git Steel1",
/*6*/ "Git Steel1 sft",
/*7*/ "Git Steel1 hrd",
/*8*/ "Git Steel1 dbl",
/*9*/ "Git Steel1 mut",
/*10*/ "Git Steel1 bwd",
/*11*/ "Git Steel2",
/*12*/ "Git Steel2 sft",
/*13*/ "Git Steel3",
/*14*/ "Git Steel3 sft",
/*15*/ "Banjo",
/*16*/ "Banjo Slide",
/*17*/ "Mandoline",
/*18*/ "Mand EnsmbTrem",
/*19*/ "Mand Ensemble",
/*20*/ "Mand EnsmSplit",
/*21*/ "Git Jazz 1",
/*22*/ "Git Jazz 2",
/*23*/ "Git JazzOctave",
/*24*/ "Pdl Steel vibr",
/*25*/ "Pedal Steel",
/*26*/ "Pdl Steel bowd",
/*27*/ "Pdl Steel slid",
/*28*/ "Gui Jazz 3",
/*29*/ "G. Jz3 Chicken",
/*30*/ "Git Jazz 4",
/*31*/ "G. Jz4 Chicken",
/*32*/ "E-Git clean 1",
/*33*/ "E-Git mute 1",
/*34*/ "E-Git clean 2",
/*35*/ "Guitar mute 2",
/*36*/ "E-Git Overdriv",
/*37*/ "E-Git Distortn",
/*38*/ "Rhythm Guitar",
/*39*/ "Rhythm Guitar",
"-> 123","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","<- 39",
/*123*/ "Nylon Percus",
/*124*/ "Guitar stroke",
/*125*/ "Guitar noise",
/*126*/ "Guitar ghost",
/*127*/ ""
};
// [Accordion]
PROGMEM const char Bank8Names[][MAX_NAME_LEN+1] = {
/*0*/ "Harmonica",
/*1*/ "Harmonic slide",
/*2*/ "Harmonic vibra",
/*3*/ "Accordina",
/*4*/ "Musette 888",
/*5*/ "Musette 888+16",
/*6*/ "Musette 4+888",
/*7*/ "Muste 4+888+16",
/*8*/ "Vintage Italo",
/*9*/ "Musette VM 888",
/*10*/ "Mste VM 888+16",
/*11*/ "Muste VM 4+888",
/*12*/ "Ms.VM 4+888+16",
/*13*/ "888 Benelux",
/*14*/ "888 Germany",
/*15*/ "888 Italy",
/*16*/ "888 French",
/*17*/ "88 French",
/*18*/ "88+16",
/*19*/ "4+88",
/*20*/ "4+88+16",
/*21*/ "Tango 8+16",
/*22*/ "Tango 8+16 rpt",
/*23*/ "Clst 88 French",
/*24*/ "Celeste 88+16",
/*25*/ "Celest 4+88+16",
/*26*/ "Clst VE10 88 F",
/*27*/ "Cls VE10 88+16",
/*28*/ "SuperVI Reed 8",
/*29*/ "S.VI Reed 4+8",
/*30*/ "S.VI Reed 88",
/*31*/ "S.VI Rd. 4+16",
/*32*/ "S.VI Rd. 4+88",
/*33*/ "S.VI Rd. 8+16",
/*34*/ "S.VI Rd.4+8+16",
/*35*/ "S.VI Rd. 88+16",
/*36*/ "S.VI R.4+88+16",
/*37*/ "Big Musette",
/*38*/ "Accordion 16",
/*39*/ "Accordion 8+16",
/*40*/ "Accord. 4+8+16",
/*41*/ "Accordion 4+16",
/*42*/ "Accordion 888",
/*43*/ "Accordion 888+",
/*44*/ "Accord. 888++",
/*45*/ "Accord. 888+++",
/*46*/ "Accordion 8",
/*47*/ "Accordion 8+16",
/*48*/ "Accord. 4+8+16",
/*49*/ "Accord. 4+8",
/*50*/ "Melodeon 8",
/*51*/ "Melod. 88 flat",
/*52*/ "Melod. 88 med.",
/*53*/ "Melod. 88 wide",
/*54*/ "Melodeon 816",
/*55*/ "M. Steirische",
/*56*/ "Melod. Cororna",
/*57*/ "Melod. Saturn",
/*58*/ "Melod. Örgeli",
/*59*/ "Melod. Serena",
"-> 118","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","<- 59",
/*118*/ "Elec. VOX4",
/*119*/ "Elec. VOX4",
/*120*/ "Acc Bass Chord",
/*121*/ "Melodeon Chord",
/*122*/ "Melodeon Bass",
/*123*/ "M. BassHelikon",
/*124*/ "Attack noise",
/*125*/ "Atck noise lin",
/*126*/ "Bellow noise",
/*127*/ ""
};
// [Full Strings & Disco Strings]
PROGMEM const char Bank9Names[][MAX_NAME_LEN+1] = {
/*0*/ "FullStr1 Emotn",
/*1*/ "FullStr1 short",
/*2*/ "FullStr1 medi",
/*3*/ "FullStr1 long",
/*4*/ "FullStr1 slow",
/*5*/ "FullStr2 short",
/*6*/ "FullStr2 medi",
/*7*/ "FullStr2 long",
/*8*/ "FullStr2 forte",
/*9*/ "FullStr2 piano",
/*10*/ "FullStr3 Emotn",
/*11*/ "FullStr3 FAD",
/*12*/ "FullStr3 slow",
/*13*/ "FullStr3",
/*14*/ "FullStr tremol",
/*15*/ "FullStr pizzic",
/*16*/ "DiscoStr long",
/*17*/ "DiscoStr short",
/*18*/ "DiscoStr glide",
/*19*/ "DiscoStr slide",
/*20*/ "VnE. vel s arc",
/*21*/ "VnE.velo split",
/*22*/ "VnE.piano only",
/*23*/ "Vlin.Ens. slow",
/*24*/ "Vn.E.pi.o.slow",
/*25*/ "Vn.Ens.detache",
/*26*/ "Vn.Ens. legato",
/*27*/ "Vn.Ens.sordino",
/*28*/ "Vn.Ens.tremolo",
/*29*/ "Vn.Ens.pizzict",
/*30*/ "VaE. vel s arc",
/*31*/ "VaE.velo split",
/*32*/ "VaE.piano only",
/*33*/ "Viola Ens.slow",
/*34*/ "Va.E.pi.o.slow",
/*35*/ "Va.Ens.detache",
/*36*/ "Va.Ens. legato",
/*37*/ "Va.Ens.sordino",
/*38*/ "Va.Ens.tremolo",
/*39*/ "Va.Ens.pizzict",
/*40*/ "CeE. vel s arc",
/*41*/ "CeE.velo split",
/*42*/ "<- ->",
/*43*/ "Cello Ens.slow",
/*44*/ "<- ->",
/*45*/ "C.Ens. detache",
/*46*/ "C.Ens. legato",
/*47*/ "<- ->",
/*48*/ "C.Ens. tremolo",
/*49*/ "C.Ens. pizzict",
/*50*/ "CBs. vel s arc",
/*51*/ "CBs.velo split",
/*52*/ "<- ->",
/*53*/ "Contra Bs.slow",
/*54*/ "<- ->",
/*55*/ "CBs.E. detache",
/*56*/ "CBs.E. legato",
/*57*/ "<- ->",
/*58*/ "CBs.E. tremolo",
/*59*/ "CBs.E. pizzict",
"-> 125","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","<- 59",
/*125*/ "StrEns.1 detch",
/*126*/ "StrEns.2 detch",
/*127*/ ""
};
// [Solo Strings]
PROGMEM const char Bank10Names[][MAX_NAME_LEN+1] = {
/*0*/ "Geige",
/*1*/ "Geige soft",
/*2*/ "Geige legato",
/*3*/ "Geige leg.up",
/*4*/ "Geige leg.down",
/*5*/ "Geige slide",
/*6*/ "Celtic Fiddle",
/*7*/ "Celtic F. arco",
/*8*/ "Celtic F.slide",
/*9*/ "Violin velodyn",
/*10*/ "Violin marcato",
/*11*/ "Violin slow",
/*12*/ "Violin detache",
/*13*/ "Violin legato",
/*14*/ "Violin tremolo",
/*15*/ "Violin pizzict",
/*16*/ "Viola velodyn",
/*17*/ "Viola marcato",
/*18*/ "Viola slow",
/*19*/ "Viola detache",
/*20*/ "Viola legato",
/*21*/ "Viola tremolo",
/*22*/ "Viola pizzicat",
/*23*/ "Cello velodyn",
/*24*/ "Cello marcato",
/*25*/ "Cello slow",
/*26*/ "Cello detache",
/*27*/ "Cello legato",
/*28*/ "Cello tremolo",
/*29*/ "Cello pizzicat",
/*30*/ "CBass velodyn",
/*31*/ "CBass marcato",
/*32*/ "ContraBss.slow",
/*33*/ "CBass detache",
/*34*/ "CBass legato",
/*35*/ "CBass tremolo",
/*36*/ "CBass pizzicat",
"","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Synth Strings]
PROGMEM const char Bank11Names[][MAX_NAME_LEN+1] = {
/*0*/ "StringsPWM",
/*1*/ "StringsPWM up",
/*2*/ "StringsPWM ENV",
/*3*/ "Strn.PWM notch",
/*4*/ "Strings MKS",
/*5*/ "Strings MKS II",
/*6*/ "Strn. MKS LP24",
/*7*/ "Strn. MKS Rise",
/*8*/ "Strn. Solino",
/*9*/ "Stringmaster",
/*10*/ "Strings M12",
/*11*/ "Strings M12",
/*12*/ "Strings M12",
/*13*/ "Strn.M12 notch",
/*14*/ "Strn.M12 notch",
/*15*/ "Strn.M12 notch",
/*16*/ "Chroma LPF",
/*17*/ "Chroma Notch",
/*18*/ "Chroma BPF",
/*19*/ "Chroma HPF",
/*20*/ "Strn. MKS fast",
/*21*/ "Strn. MKS medi",
/*22*/ "Strn. MKS slow",
/*23*/ "Analog St.fast",
/*24*/ "Analog St.medi",
/*25*/ "Analog St.slow",
/*26*/ "JP Strngs.fast",
/*27*/ "JP Strngs.medi",
/*28*/ "JP Strngs.slow",
/*29*/ "JP Strngs. BPF",
/*30*/ "MKS Strn. fast",
/*31*/ "MKS Strn. medi",
/*32*/ "MKS Strn. slow",
/*33*/ "OB Strn.fast",
/*34*/ "OB Strn.slow",
/*35*/ "OB Strn.II LPF",
/*36*/ "OB Strn.II BPF",
/*37*/ "OB Strn.II HPF","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Brass Solo]
PROGMEM const char Bank12Names[][MAX_NAME_LEN+1] = {
/*0*/ "Trp1 velosplt1",
/*1*/ "Trp1 velosplt2",
/*2*/ "Trumpet1 piano",
/*3*/ "Trumpet1 mezzo",
/*4*/ "Trumpet1 forte",
/*5*/ "Trumpet1 fall",
/*6*/ "Trumpet1 goup",
/*7*/ "Trumpet1 slide",
/*8*/ "Trumpet 2 Ivan",
/*9*/ "Trp2 velo splt",
/*10*/ "Trp2 1/4 note",
/*11*/ "Trp2 1/4 vlspl",
/*12*/ "Trumpet3 vibrt",
/*13*/ "Trp3 vibr soft",
/*14*/ "Trp3 vibr hard",
/*15*/ "Trp3 vibrato 2",
/*16*/ "Trp3 vibrt sfz",
/*17*/ "Trp3 viblegato",
/*18*/ "Trt3 high lead",
/*19*/ "Cornet",
/*20*/ "Cornet soft",
/*21*/ "Cornet hard",
/*22*/ "C. portam.atck",
/*23*/ "Trumpet mute",
/*24*/ "Trumpet muteAV",
/*25*/ "Flugelhorn",
/*26*/ "Trombone vibrt",
/*27*/ "Trombone",
/*28*/ "Trombone dampr",
/*29*/ "F-Horn",
/*30*/ "Tenorhorn",
/*31*/ "Tenorhorn vbrt",
/*32*/ "Alphorn",
/*33*/ "Baritone vibrat",
/*34*/ "Baritone stacct",
/*35*/ "Tuba",
/*36*/ "Tuba soft","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Brass Section]
PROGMEM const char Bank13Names[][MAX_NAME_LEN+1] = {
/*0*/ "Trumpet Sect.",
/*1*/ "Trp.Sect.forte",
/*2*/ "Trp.Sect.mezzo",
/*3*/ "Trp.S.fst.fall",
/*4*/ "Trp.S.med.fall",
/*5*/ "Trp.Sect.split",
/*6*/ "Trombone Sect.",
/*7*/ "Trb.Sect.forte",
/*8*/ "Trb.Sect.mezzo",
/*9*/ "Trb.S.fst.fall",
/*10*/ "Trb.S.med.fall",
/*11*/ "Trb.Sect.split",
/*12*/ "Trp.&Trb.Sect.",
/*13*/ "Trp.&Trb.S.frt",
/*14*/ "Trp.&Trb.S.mez",
/*15*/ "Flugelhorn Duo",
/*16*/ "Tenorh.Ens.vib",
/*17*/ "Alphorn Duett",
/*18*/ "Alphorn Trio",
/*19*/ "Alphorn Ensemb","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Classic Brass]
PROGMEM const char Bank14Names[][MAX_NAME_LEN+1] = {
/*0*/ "Trumpet",
/*1*/ "Trumpet piano",
/*2*/ "Trumpet stacct",
/*3*/ "Trombone",
/*4*/ "Trombone piano",
/*5*/ "Trombone stacc",
/*6*/ "Horn",
/*7*/ "Horn piano",
/*8*/ "Horn staccato",
/*9*/ "Tuba",
/*10*/ "Tuba piano",
/*11*/ "Tuba staccato",
/*12*/ "Full Br.velspl",
/*13*/ "Full Br. piano",
/*14*/ "Trp.Ens.velspl",
/*15*/ "Trp.Ens. piano",
/*16*/ "Trp.Ens.stacct",
/*17*/ "Trb.Ens.velspl",
/*18*/ "Trb.Ens. piano",
/*19*/ "Trb.Ens.stacct",
/*20*/ "HornEns.velspl",
/*21*/ "HornEns. piano",
/*22*/ "HornEns.stacct",
/*23*/ "Tuba Ensemble",
"-> 126","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","<- 23",
/*126*/ "HornEns. riff",
/*127*/ ""
};
// [Saxophon]
PROGMEM const char Bank15Names[][MAX_NAME_LEN+1] = {
/*0*/ "Soprano Sax",
/*1*/ "SSax softer",
/*2*/ "SSax harder",
/*3*/ "SSax noise",
/*4*/ "SSax side",
/*5*/ "SSax II vibrat",
/*6*/ "Alto Sax",
/*7*/ "ASax softer",
/*8*/ "ASax harder",
/*9*/ "ASax breath",
/*10*/ "ASax slide",
/*11*/ "Tenor Sax",
/*12*/ "TSax softer",
/*13*/ "TSax harder",
/*14*/ "TSax breath",
/*15*/ "TSax slide",
/*16*/ "Max Jz Tenor 1",
/*17*/ "Max Jz Tenor 2",
/*18*/ "Max JT soft",
/*19*/ "Max Jz Tenor 3",
/*20*/ "Max JT vibrato",
/*21*/ "Max JT soft",
/*22*/ "Max JT slide",
/*23*/ "Max JT sld sft",
/*24*/ "TSax funky",
/*25*/ "TSax fky growl",
/*26*/ "TSax fky split",
/*27*/ "Baritone Sax",
/*28*/ "Sax Section 1",
/*29*/ "Sax Section 2",
"-> 126","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","<- 29",
/*126*/ "Sax Breath",
/*127*/ ""
};
// [Winds]
PROGMEM const char Bank16Names[][MAX_NAME_LEN+1] = {
/*0*/ "Clarinet",
/*1*/ "Clarinet soft",
/*2*/ "Clarinet slide",
/*3*/ "Hugo Clarinet",
/*4*/ "Hugo Clarinet slide",
/*5*/ "Clr Tradition",
/*6*/ "Clr Tradition",
/*7*/ "Clr Trad soft",
/*8*/ "Clr Trad stacc",
/*9*/ "Clr Trad stacc",
/*10*/ "Piccolo",
/*11*/ "Flute",
/*12*/ "Flute EQ",
/*13*/ "Flute highpass",
/*14*/ "Low Whistle",
/*15*/ "Panflute",
/*16*/ "Shakuhachi",
/*17*/ "Celtic Whistle",
/*18*/ "C.Wh. auto grc",
/*19*/ "Clt. Wh. slide",
/*20*/ "Bottle",
/*21*/ "Bottle Q",
/*22*/ "Bottle LFO",
/*23*/ "Whistle",
/*24*/ "Okarina",
/*25*/ "Highland Drone",
/*26*/ "Ullian Drone",
/*27*/ "U.Drn auto grc",
/*28*/ "U.Drn slide",
/*29*/ "U.Drn chords",
/*30*/ "Border Pipe",
/*31*/ "B.Pip auto grc",
/*32*/ "B. Pipe slide",
/*33*/ "B. Pipe Drone","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Classic Winds]
PROGMEM const char Bank17Names[][MAX_NAME_LEN+1] = {
/*0*/ "Classic Flute",
/*1*/ "Cls. Flt piano",
/*2*/ "Cls. Flt stacc",
/*3*/ "Oboe",
/*4*/ "Oboe piano",
/*5*/ "Oboe staccato",
/*6*/ "English Horn",
/*7*/ "Engl. Hrn pno",
/*8*/ "Engl. Hrn stac",
/*9*/ "Cls. Clarinet",
/*10*/ "Cls. Clr pno",
/*11*/ "Cls. Clr stac",
/*12*/ "Bassoon",
/*13*/ "Bassoon piano",
/*14*/ "Bassoon stacc",
/*15*/ "Bass Clarinet",
/*16*/ "Contra Bassoon",
/*17*/ "Flute Ensemble",
/*18*/ "Clar. Ensemble","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Choir]
PROGMEM const char Bank18Names[][MAX_NAME_LEN+1] = {
/*0*/ "Choir Aah",
/*1*/ "Choir Aah filt",
/*2*/ "Choir Ooh",
/*3*/ "Choir Ooh filt",
/*4*/ "Pop Ooh",
/*5*/ "Pop Ooh filter",
/*6*/ "Synth Voice",
/*7*/ "Classic Voice",
/*8*/ "Boys Aah",
/*9*/ "Boys Bap",
/*10*/ "Boys Daa",
/*11*/ "Boys Doo Bass",
/*12*/ "Boys Doo",
/*13*/ "Boys Fals Ooh",
/*14*/ "Boys Hmm",
/*15*/ "Boys Laa",
/*16*/ "Boys Mix Ooh",
/*17*/ "Boys Ooh",
/*18*/ "Girls Aah",
/*19*/ "Girls Doo",
/*20*/ "Girls Ooh",
/*21*/ "Voiceless",
"-> 126","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","<- 21",
/*126*/ "Voice Kit",""
};
// [Bass]
PROGMEM const char Bank19Names[][MAX_NAME_LEN+1] = {
/*0*/ "E-Bass 1",
/*1*/ "E-B.1 note-off",
/*2*/ "E-Bass 1 slide",
/*3*/ "E-Bass 2",
/*4*/ "E-B.2 note-off",
/*5*/ "E-Bass 3",
/*6*/ "E-Bass 3 slide",
/*7*/ "E-Bass 4",
/*8*/ "E-Bass 5 Pick1",
/*9*/ "E-B.5 Pck1 n-o",
/*10*/ "E-Bass 6 Pick2",
/*11*/ "E-B.6 Pick2 n-o",
/*12*/ "E-Bass 7",
/*13*/ "E-Bass 7 repeti",
/*14*/ "E-B.7 note-off",
/*15*/ "E-B. Fretless",
/*16*/ "E-Bass Slap1",
/*17*/ "E-B. Slap1 n-o",
/*18*/ "E-Bass Slap2",
/*19*/ "E-B. Slap2 n-o",
/*20*/ "Upright Jazz B",
/*21*/ "Upright Jz n-o",
/*22*/ "Up Jz velosplt",
/*23*/ "Up Jz velo n-o",
/*24*/ "Up Jz velosplt",
/*25*/ "Up Jz velo n-o",
/*26*/ "Up Jz no fingr",
/*27*/ "Up B finger at",
/*28*/ "Up B more fngr",
/*29*/ "Up B no finger",
/*30*/ "Upright Bss DI",
/*31*/ "Up B DI no fgr",
/*32*/ "Up B DI long",
/*33*/ "Up B DI lng no",
/*34*/ "Bowed Upright",
/*35*/ "Bowed Up short","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""
};
// [Synthesizer]
PROGMEM const char Bank20Names[][MAX_NAME_LEN+1] = {
/*0*/ "Bells & Pad",
/*1*/ "Digital Pad",
/*2*/ "OBX & Wavebell",
/*3*/ "DX1 Toy",
/*4*/ "Star Theme",
/*5*/ "Brightness",
/*6*/ "OB & Noise",
/*7*/ "Atmos",
/*8*/ "Brass Comp",
/*9*/ "Brass Rex",
/*10*/ "Polysynth Clsc",
/*11*/ "Halo Pad",
/*12*/ "Caliope",
/*13*/ "Charang",
/*14*/ "Fairly Space",
/*15*/ "Echo Drop",
/*16*/ "VF Vox",
/*17*/ "Bass & Lead",
/*18*/ "Fantasia",
/*19*/ "Bowed Glass",
/*20*/ "Soft Pad",
/*21*/ "Ice Rain",
/*22*/ "Goblin",
/*23*/ "Sound Track",
/*24*/ "Atmos Guitar",
/*25*/ "Bottle soft",
/*26*/ "Polys.Clsc 5th",
/*27*/ "Square Lead",
/*28*/ "Prophet5 Brass",
/*29*/ "Saw",
/*30*/ "Saw Env",
/*31*/ "C-Lead",
/*32*/ "Solo Vox",
/*33*/ "Metal Pad",
/*34*/ "Juno Sweep",
/*35*/ "Vang Brass",
/*36*/ "Crystal",
/*37*/ "FM 8",
/*38*/ "Mo55",
/*39*/ "DX Bell",
/*40*/ "OB S1",
/*41*/ "OB Soft",
/*42*/ "Hook",
/*43*/ "FM Pluk",
/*44*/ "FM Brazz",
/*45*/ "Ice",
/*46*/ "Bo Hook",
/*47*/ "V Phrase",
/*48*/ "VP 1",
/*49*/ "Grace",
/*50*/ "Noise",
/*51*/ "Digirace",
/*52*/ "Shinner",
/*53*/ "Pad-A",
/*54*/ "Vibro",
/*55*/ "Digisi",
/*56*/ "Alex",
/*57*/ "VZ Bell",
/*58*/ "VZ 1",
/*59*/ "Mizoo",
/*60*/ "Bellko",
/*61*/ "Bellz",
/*62*/ "Analog OB",
/*63*/ "M3 Osc",
/*64*/ "M12 Brass",
/*65*/ "M12 Brass ENV",
/*66*/ "OB Lead",
/*67*/ "OB Arp",
/*68*/ "OB Bell",
/*69*/ "OB Brass",
/*70*/ "Mach 1",
/*71*/ "Brazza",
/*72*/ "Brasso",
/*73*/ "T8 SuperBrass",
/*74*/ "T8 SuperBr.ENV",
/*75*/ "Dells",
/*76*/ "Pulse",
/*77*/ "Pulso",
/*78*/ "PWD 24",
/*79*/ "Xypho",
/*80*/ "Charpo",
/*81*/ "Jippo",
/*82*/ "JX Arp",
/*83*/ "Bamarimba",
/*84*/ "JCO 10",
/*85*/ "JX Bell",
/*86*/ "Stab Brass",
/*87*/ "Clouds",
/*88*/ "Bell Hit HiPss",
/*89*/ "OB Noise",
/*90*/ "Noise down",
/*91*/ "Voxo notch 12",
/*92*/ "APad notch 12",
/*93*/ "PWD notch 12",
/*94*/ "MOSC notch 12",
/*95*/ "Square bndp 24",
/*96*/ "OXB bndpass 24",
/*97*/ "Hook bndpss 24",
/*98*/ "Pulso bndps 24",
/*99*/ "Jops bndpss 12",
/*100*/ "Filtr bndps 24",
/*101*/ "PolyS bndps 24",
/*102*/ "OBLead bndp 24",
/*103*/ "JX Bll ntch 24",
/*104*/ "Barimbo bdp 24",
/*105*/ "Arp bndpass 24",
/*106*/ "Bottle hips 12",
/*107*/ "Jells bndps 24",
/*108*/ "Poly hipass 24",
/*109*/ "Mogo hipass 24",
/*110*/ "HBells hips 24",
/*111*/ "HBelly hips  6",
/*112*/ "HBrass hips 24",
/*113*/ "ClsPoly hip 12",
/*114*/ "Harppo hips 24",
/*115*/ "Hipps hipss  6",
/*116*/ "Classic S.Bass",
/*117*/ "Clsc S.Bs.Rezo",
/*118*/ "J Bass 1",
/*119*/ "J Bass 2",
/*120*/ "J Bass 3",
/*121*/ "J Bass soft",
/*122*/ "CS Classc Bass",
/*123*/ "Moog Bass",
/*124*/ "Moog Bass ENV",
/*125*/ "X Bass 1",
/*126*/ "X Bass 2",
/*127*/ ""
};
// [Specials]
PROGMEM const char Bank21Names[][MAX_NAME_LEN+1] = {
/*0*/ "Orch Hit major",
/*1*/ "Orch Hit minor",
"-> 62","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","<- 1",
/*62*/ "Synth FX down",
/*63*/ "Synth FX up",
/*64*/ "Drum Kit 0",
/*65*/ "Percssn Kit 56",
/*66*/ "Cls Prc Kit 48",
"-> 70","","<- 66",
/*70*/ "Bass Dr Kit 0",
/*71*/ "Bass Dr Kit 1",
/*72*/ "Bass Dr Kit 2",
/*73*/ "Bass Dr Kit 3",
/*74*/ "Bass Dr Kit 4",
/*75*/ "Bass Dr Kit 5",
/*76*/ "Bass Dr Kit 6",
/*77*/ "Bass Dr Kit 7",
/*78*/ "Bass Dr Kit 8",
/*79*/ "Bass Dr Kit 9",
/*80*/ "Bass Dr Kit 10",
/*81*/ "Bass Dr Kit 11",
/*82*/ "Bass Dr Kit 12",
/*83*/ "Bass Dr Kit 13",
/*84*/ "Bass Dr Kit 14",
/*85*/ "Bass Dr Kit 15",
/*86*/ "Bass Dr Kit 16",
/*87*/ "Bass Dr Kit 17",
/*88*/ "Bass Dr Kit 18",
/*89*/ "Bass Dr Kit 19",
/*90*/ "Bass Dr Kit 32",
/*91*/ "Bass Dr Kit 33",
/*92*/ "Bass Dr Kit 34",
/*93*/ "Bass Dr Kit 35",
/*94*/ "Bass Dr Kit 48",
/*95*/ "Bass Dr Kit 49",
/*96*/ "Bass Dr Kit 24",
/*97*/ "Bass Dr Kit 25",
/*98*/ "Bass Dr Kit 26",
/*99*/ "Bass Dr Kit 27",
/*100*/ "Bass Dr Kit 28",
/*101*/ "Bass Dr Kit 29",
/*102*/ "Bass Dr Kit 30",
/*103*/ "Bass Dr Kit 31",
"","","","","","","","",
"","","","","","","","","","","","","","","",""
};

char name_buffer[MAX_NAME_LEN+1]; 

const char * toString(SoXXLBank bank, byte program_number) {
  if (program_number > 127)
    return "prg > 127?";
  switch (bank) {
    case GeneralMIDI:
      strcpy_P(name_buffer, &(Bank0Names[program_number][0]));
      break;
    case Piano:
      strcpy_P(name_buffer, &(Bank1Names[program_number][0]));
      break;
    case EPiano:
      strcpy_P(name_buffer, &(Bank2Names[program_number][0]));
      break;
    case Organ:
      strcpy_P(name_buffer, &(Bank3Names[program_number][0]));
      break;
    case Drawbar:
      strcpy_P(name_buffer, &(Bank4Names[program_number][0]));
      break;
    case PercTuned:
      strcpy_P(name_buffer, &(Bank5Names[program_number][0]));
      break;
    case StringInstr:
      strcpy_P(name_buffer, &(Bank6Names[program_number][0]));
      break;
    case Guitar:
      strcpy_P(name_buffer, &(Bank7Names[program_number][0]));
      break;
    case Accordion:
      strcpy_P(name_buffer, &(Bank8Names[program_number][0]));
      break;
    case Strings:
      strcpy_P(name_buffer, &(Bank9Names[program_number][0]));
      break;
    case StringsSolo:
      strcpy_P(name_buffer, &(Bank10Names[program_number][0]));
      break;
    case StringsSynth:
      strcpy_P(name_buffer, &(Bank11Names[program_number][0]));
      break;
    case BrassSolo:
      strcpy_P(name_buffer, &(Bank12Names[program_number][0]));
      break;
    case BrassSection:
      strcpy_P(name_buffer, &(Bank13Names[program_number][0]));
      break;
    case BrassClassic:
      strcpy_P(name_buffer, &(Bank14Names[program_number][0]));
      break;
    case Saxophon:
      strcpy_P(name_buffer, &(Bank15Names[program_number][0]));
      break;
    case Winds:
      strcpy_P(name_buffer, &(Bank16Names[program_number][0]));
      break;
    case WindsClassic:
      strcpy_P(name_buffer, &(Bank17Names[program_number][0]));
      break;
    case Choir:
      strcpy_P(name_buffer, &(Bank18Names[program_number][0]));
      break;
    case Bass:
      strcpy_P(name_buffer, &(Bank19Names[program_number][0]));
      break;
    case Synth:
      strcpy_P(name_buffer, &(Bank20Names[program_number][0]));
      break;
    case FXnPerc:
      strcpy_P(name_buffer, &(Bank21Names[program_number][0]));
      break;
    default:
      return "bank?"; // unknown bank
  }
  return name_buffer;
}

struct SoXXLMessage {
  byte buff[12];
  int length;
};

SoXXLMessage SoXXL_msg;

enum SoXXLNRPN {
  PitchBendSensitivity, FineTuning, CoarseTuning
};

SoXXLMessage toNRPNMsg(SoXXLNRPN nrpn, byte channel, byte value) {
  SoXXL_msg.length = 7;
  SoXXL_msg.buff[0] = 0xB0 | channel;
  SoXXL_msg.buff[1] = 0x65;
  SoXXL_msg.buff[2] = 0x00;
  SoXXL_msg.buff[3] = 0x64;
  SoXXL_msg.buff[5] = 0x06;
  SoXXL_msg.buff[6] = value;
  switch (nrpn) {
    case PitchBendSensitivity: 
      SoXXL_msg.buff[4] = 0x00; 
      break;
    case FineTuning: 
      SoXXL_msg.buff[4] = 0x01; 
      break;
    case CoarseTuning: 
    default:
      SoXXL_msg.buff[4] = 0x02; 
      break;
  }
  return SoXXL_msg;
}

/**
 * Master volume 0..127.
 */
SoXXLMessage toVolumeMsg(byte value) {
  SoXXL_msg.length = 8;
  SoXXL_msg.buff[0] = 0xf0;
  SoXXL_msg.buff[1] = 0x7f;
  SoXXL_msg.buff[2] = 0x7f;
  SoXXL_msg.buff[3] = 0x01;
  SoXXL_msg.buff[4] = 0x01;
  SoXXL_msg.buff[5] = 0x00;
  SoXXL_msg.buff[6] = value;
  SoXXL_msg.buff[7] = 0xf7;
  return SoXXL_msg;
}

SoXXLMessage toReverbTypeMsg(byte type) {
  SoXXL_msg.length = 11;
  SoXXL_msg.buff[0] = 0xf0;
  SoXXL_msg.buff[1] = 0x41;
  SoXXL_msg.buff[2] = 0x00;
  SoXXL_msg.buff[3] = 0x42;
  SoXXL_msg.buff[4] = 0x12;
  SoXXL_msg.buff[5] = 0x40;
  SoXXL_msg.buff[6] = 0x7f;
  SoXXL_msg.buff[7] = 0x7f;
  SoXXL_msg.buff[8] = type;
  SoXXL_msg.buff[9] = 0x00;
  SoXXL_msg.buff[10] = 0xf7;
  return SoXXL_msg;
}

SoXXLMessage toEffectTypeMsg(byte type) {
  SoXXL_msg.length = 11;
  SoXXL_msg.buff[0] = 0xf0;
  SoXXL_msg.buff[1] = 0x41;
  SoXXL_msg.buff[2] = 0x00;
  SoXXL_msg.buff[3] = 0x42;
  SoXXL_msg.buff[4] = 0x12;
  SoXXL_msg.buff[5] = 0x40;
  SoXXL_msg.buff[6] = 0x01;
  SoXXL_msg.buff[7] = 0x38;
  SoXXL_msg.buff[8] = type;
  SoXXL_msg.buff[9] = 0x00;
  SoXXL_msg.buff[10] = 0xf7;
  return SoXXL_msg;
}

