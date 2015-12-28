/*--------------------------------- Ketron SD-2 sound module ---------------------------------*/

/* Ketron SD-2 voice banks with control change values */
enum SD2Bank {
  Presets = 2,
  BankA = 0,
  BankB = 1,
  BankC = 10,
  Drums = 4 
};

SD2Bank nextBank(SD2Bank current);
SD2Bank prevBank(SD2Bank current);

SD2Bank nextBank(SD2Bank current) {
  switch (current) {
    case Presets: return BankA;
    case BankA: return BankB;
    case BankB: return BankC;
    case BankC: return Drums;
    case Drums: return Presets;
  }
  return Presets;
}

SD2Bank prevBank(SD2Bank current) {
  switch (current) {
    case Presets: return Drums;
    case BankA: return Presets;
    case BankB: return BankA;
    case BankC: return BankB;
    case Drums: return BankC;
  }
  return Presets;
}

/**
 * Returns the name of the SD-2 sound bank as string.
 */
String toString(SD2Bank bank) {
  switch (bank) {
    case Presets: return "Presets";
    case BankA: return "BankA";
    case BankB: return "BankB";
    case BankC: return "BankC";
    case Drums: return "Drums";
  }
}

const char PresetNames[][15] = {
// Piano
"Grand Piano","Rock Piano","Honky Tonk","Clavinet",
// Electric Piano
"Vintage Piano","Rhodes","Stage Piano","DX Piano","ELO Piano",
// Chrom
"Long Vibes","Marimba",
// Organ
"Leslies","Jazz Organ 1","Jazz Organ 2","Rock Organ B3","Theatre Organ","Rotary","Whiter","Rock Slow","Rock Fast","Jazzy Organ","Gospel Organ","Pipe Organ","Positive",
// Accordion
"Musette","Accordion","Master","Francaise","Cassotto","Fisa","Tango","Diatonic","Alpen","Harmonica",
// Acoustic Guitar
"Classic Gt.","Country Gt.","Takamin","Django","Spanish Gt.","Folk Gt.","Nylon Gt.","Brazilian Gt.","Fingerpick","12 Strings",
// Electric Guitar
"Strato","Jazz Gt.","Clean Gt.","Pop Gt.","Overdrive","Texas Gt.","Muted Gt.","Solid Honky","Blues Gt.","60' Stopped","Hawaian Gt.","Telecast","Lead Guitar","Wha Guitar",
// Strings
"101 Strings","Violin 1","Violin 2","Fast Strings","Wiener","Harp","Tzivago","Strings 1","Strings 2","Strings 3",
// Choir
"Choir","Dah Choir","Aah Choir","Tuuh Choir","Uuh Choir","Synvoice","Vocals","Ooh Choir",
// Brass
"Trumpet 1","Trumpet 2","Golden Tpt","Trombone 1","Trombone 2","Miles Davis","Brass","Dixie Tpt","Flugelhorn","Mute Tpt","French Horn","Octabrass","Synbrass",
// Sax
"Altosoft","Jazz Tenor","Contralto","Rock Tenor","Liscio Sax","Blowed Sax","Rock Sax","Night Sax","Bariton Sax","Clarinet",
// Flute
"Flute","Andes","Recorder","Shakuhashi","Whistle",
// Synth
"Sawmix","Brassfx","Synclead","Chick","Minimoog","Octasaw","Patlead","Maxisynth","Squares",
// Synth Pad
"Warmness","Magic","Glockpad","Whisper","Morphing","Ambience","Wiring","Newage","Large",
// Ethnic
"Banjo","Mandolin","Sitar","Cuatro","Hackbrett","Peruvian"
};

const char BankANames[][15] = {
"Grand Piano","Rock Piano","Upright Piano","Honky Tonk","Rhodes","DX Piano","Harpsichord","Clavinet",
"Celesta","Glocken","Music Box","Vibraphon","Marimba","Xylophon","Bell","Santur",
"Leslies","Jazz Organ 1","Rock Organ B3","Church Organ","Theatre Organ","Musette","Harmonica","Accordion",
"Classic Gt.","Country Gt.","Jazz Gt.","Clean Gt.","Muted Gt.1","Overdrive","Blues Gt.","Harmonics",
"Jazz Bass","Finger Bs.","Picked Bs.","Fretless Bs.","Slap Bass","Funk Bass","Synth.Bass 1","Synth.Bass 2",
"Violin 1","Viola","Cello","Contrabass","Tremolos","Pizzicato","Harp","Timpani",
"Strings 1","Slow Strings","Syn.Strings 1","Syn.Strings 2","Choir 1","Choir 2","Synth.Voice","Hits",
"Trumpet 1","Trombone 1","Tuba","Miles Davis","French Horn","Brass 1","Synbrass 1","Synbrass 2",
"Altosoft","Contralto","Rock Tenor","Bariton Sax","Oboe","English Horn","Bassoon","Clarinet",
"Piccolo","Flute","Recorder","Andes","Bottle","Shakuhashi","Whistle","Ocarina",
"Squares","Saw","Calliope","Wha","Plate","Vox","Saw 5th","FM Lead",
"Fantasy","Warmpad","Score","Space","Softpad","Metal","Halo","Sweep",
"Ice Rain","Soundtrack","Crystal","Atmosphere","Brightness","Goblin","Echodrops","Startheme",
"Sitar","Banjo","Shamisen","Koto","Kalimba","Bagpipe","Fiddle","Zurna",
"Tinkle","Agogo","Steeldrum","Woodblock","Taiko","Melotom","Syntom","Reverse",
"Fretslide","Breath","Seashore","Bird","Telephone","Helicopter","Applause","Gun Shot"
};

const char BankBNames[][15] = {
"Jingle","Electric","House","ELO Piano","Vintage","Funky Piano","FM Piano 1","FM Piano 2",
"Toy Box","Chimes","Mallets","Long Vibes","Malimba","Mellow","Tinkles","Wind Chimes",
"Rotary","Jazz Organ 2","Drawbar","Positive","Master","Francaise","Cassotto","Tango",
"Nylon Gt.","Folk Gt.","Funk Gt.","60' Stopped","Muted Gt.2"," Lead Guitar","12 Strings","Solid"
"Warm","Ms Finger","Mute Bass","Bluebass","Precision Bs.","Synth.Bass 3","Synth.Bass 4","Synth.Bass 5",
"Philarmoni","Violas","GlockString","Octa Strings 1","Octa Strings 2","Marcato","Orchestra","Slow Arcos",
"Strings 2","Dark Strings","Dah Choir","Vocalize","Scat Choir","Ooh Choir","Aah Choir","Chorale",
"Golden Tpt","Cornet","Tuba&Bass","Muted Tpt","Flugelhorn 1","Brass 2","Grow Brass 1","Brass Fx",
"Soprano","Liscio Sax","Jazz Tenor","Graffiato","Mariachi","Grow Brass 2","Blare","Slide Tbn",
"Clarino","Fife","Lip Flute","Chiff","Analogs","Flanger","Brightpad","Wiring",
"Wow","Starcase","Square","Sine","Rock Bass","Synth.Bass 6","Synth.Bass 7","Synth.Bass 8",
"2nd Perc","3th Perc","Click","Tonebar","Pop Organ","Jazz Organ 3","Lower 8' - 4'","Jazzy",
"Pop Gt.","Telecast","Strato","Texas Gt.","Tremolo","Unplug","Stops","Wha Guitar",
"Hawaian Gt.","Mandolin","Django","Classic Fx","Pedalsteel","Rock&Roll","Reed 8'","Blow",
"Fx","Tecnosynth 1","Tecnosynth 2","Rave 1","Rave 2","Rave 3","Rave 4","Rave 5",
"Effects 1","Effects 2","Effects 3","Effects 4","Effects 5","Effects 6","Effects 7","Effects 8"
};

const char BankCNames[][15] = {
"Grand mono","Rock mono","Piano L","Piano R","Mark","Stage Piano","Rhodes low","Rhodes high",
"Celestial","China","Air Bell","Dry Vibes","Malimba","Wood Mallet","Glock&Pad","EPiano&Strings",
"Whiter","Rock Slow","Rock Fast","Pipe Organ","Gospel Organ","Fisa","Diatonic","Alpen",
"Brazilian","Country Fx","Texas Fx","Strato Fx","Fingerpick","Pop Fx","Spanish","Takamin",
"Subbass","Bass&Guitar 1","Oberbass 1","60' Bass","Slapping","Oberbass 2","Pedalbass","Bass&Guitar 2",
"Slow Violin","Violin 2","Quartet","Tzivago","Concert","Fast Strings","Harp&Orchestra","Symphonic",
"Strings 3","Digistrings","Wiener","101 Strings","Tuuh Choir","Uuh Choir","Vocals 1","Vocals 2",
"Trumpet 2","Trombone 2","Dixie Tpt","Trombones","Fall Off","Brass 3","Latin Tbn","Octabrass",
"Blowed","Night Sax","Bebop","Rock Sax","Brasslips","Soft Trumpet","Hi Trumpet","Flugelhorn 2",
"Posaune","Anderson","Lip Flute","Panflute","Flute Vox","Ethnic Flute","Whistling","Flute&Strings",
"Sawmix","Brassfx","Synclead","Chick","Minimoog","Octasaw","Patlead","Maxisynth",
"Warmness","Sawpad","Large","Movie 1","Magic","Glockpad","Whisper","Airpad",
"Picking","Morphing","Ambience","Digipad","Movie 2","Newage","Voxing","Synfony",
"Cuatro","Hackbrett","Tres","Cajun","Peruvian","Fado","Jungle","Balama",
"Tinklebell","Triangle","Tambora","Claves","Ac.Tom","Brushtom","Snare","Crash",
"Scratch 1","Scratch 2","Scratch 3","Scratch 4","Scratch 5","Fx Slap","Laser","Wind"
};

const char LiveDrumNames[][15] = {
"Bachata","Bolero 1","Chacha 1","Cumbia 1","Guajra 1","Mambo","Salsa 1","Rumba 1",
"Merengue 1","Merengue 2","Congas 1","Congas 2","Gipsy 1","Maracas 1","Tambourine","Brush",
"","","","","Dance 1","","Ethnic 1","Ethnic 2",
"Latingr 1","Latingr 2","Latingr 3","Latingr 4","Latingr 5","Latingr 6","Latingr 7","Latingr 8",
"","","New Age","","","","Pop","",
"","Jazz 1","Shuffle 1","Jazz 2","Bossanova","Samba 1","Samba 2","Pandero 1",
"Salsa 2","Eurolatin","Popdown","","Dance 2","Dance 3","Tradition 1","Bolero 2",
"Cumbia 2","Guajra 2","Rumba 2","Merengue 3","Merengue 4","Congas 3","Congas 4","Gipsy 2",
"Maracas 2","","Jazz 3","Shuffle 2","Jazz 4","Samba 3","Samba 4","Pandero 2",
"Chacha 2","Afrohouse","Pop Jazz","Salsa 3","Tradition 2","Rock","R&Blues","Jazz Rock",
"Samba 5","Samba 6","","","","","","",
"","","","","","","","",
"","","","","","","","",
"","","","","","","","",
"","","","","","","","",
"","","","","","","",""
};

String toString(SD2Bank bank, byte program_number) {
  if (program_number > 127)
    return "> 127?";
  switch (bank) {
    case Presets: return PresetNames[program_number];
    case BankA: return BankANames[program_number];
    case BankB: return BankBNames[program_number];
    case BankC: return BankCNames[program_number];
    case Drums: return LiveDrumNames[program_number];
  }
  return "bank?"; // unknown bank
}


