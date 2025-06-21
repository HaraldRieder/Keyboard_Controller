# Elektronik für die 6+6 Tastatur

## Module

Mit dem Hauptschalter des GT-2 mini schalten sich die komplette interne Elektronik sowie die beiden gesicherten Steckdosen auf der Rückseite mit ein und aus. Für diesen Komfort sorgt ein Triac-Relais mit Nulldurchgangsschaltung. Um es anzusteuern mussten 5V Versorgungsspannung aus dem Hauptmodul des GT-2 mini abgegriffen werden.

|Blockschaltbild|
| ----------- |
|![](Blockschaltbild.PNG)|

Das GT-2 mini Hauptmodul wird wie vorgesehen von der Tastatursensorleiste angesteuert und kann über seine Original-Bedieneinheit bedient werden. Pedalsensoren sind keine angeschlossen, denn Sustain, Sostenuto usw. werden über die selbstgebaute Steuerung realisiert. Die Tastendrücke gelangen als MIDI-Nachrichten zur selbstgebauten Steuerung, die auf einem Arduino Mega 2560 basiert. An der Steuerung hängt ein 2x20 Zeilen LCD Display, MIDI-Ein- und Ausgänge, 2 Handräder für Pitch Bend und Modulation, 2 Taster und optional extern: 1 Schalter, 1 Steuerpedal, 1 Basspedal.

Die Steuerung mischt die diversen Sensorinformationen zu den MIDI-Daten, die vom GT-2 mini empfangen worden sind, und leitet das Gesamtkunstwerk über MIDI an das interne V3 Sonority XXL Sound-Modul zwecks Hörbarmachung weiter.

Der MIDI-Eingang wird durch das GT-2 mini zur Steuerung geschleift. Diese mischt sie zum MIDI-Strom Richtung V3 hinzu. Dadurch kann das V3 als Sound-Modul über den zweiten Eingang z.B. von einem Sequenzer genutzt werden, während gleichzeitig auf der Tastatur dazu gespielt wird.

Zur Bedienung der Steuerung dienen ein Regler mit benachbartem Schalter, 2 Taster (rot und schwarz), die 2 Handräder, die Tastatur (z.B. Festlegung einer Split-Position durch Tastendruck) und optional ein Fußpedal.

Der Audio-Ausgang des V3 sowie 2 externe Stereo-Eingänge (Cinch und 3,5 mm Klinke) werden über den selbstgebauten Mischer geschleift zum externen Audio-Ausgang (2 x 6,3 mm Klinke) und gleichzeitig zum Eingang des GT-2 mini, wo das gemischte Signal über die anschließbaren Kopfhörer gehört werden kann.
Auch das Metronom des GT-2 mini kann auf diese Art benutzt werden.

Der Kopfhörer-Ausgang des V3 ist direkt nach außen geführt auf eine 6,3 mm Klinkenbuchse. Dieses Signal lässt sich außer in Kopfhörer auch in Standardeingänge einspeisen.

2024 wurde ein Ketron SD-2 durch das V3 Sonority XXL Modul ersetzt. Man konnte mit dem SD-2 keine Tremolos spielen, weil es dann Noten-Aussetzer bekam, und ein Software-Update war nicht mehr zu erwarten. Mit dem V3 kann man Tremolos spielen, so wie auf jedem mechanischen Klavier.

## Arduino Mega 2560 mit Peripherie

Zunächst einmal gab es da ein schönes Wersi-Basspedal, das anschließbar sein sollte - selbstverständlich ein 6+6 Pedal. Es hing [früher](https://github.com/HaraldRieder/Historic/index.html) an einer eigens dafür aufgebohrten Döpfer LMK3-Elektronik und enthält eine zum LMK3 passende Diodenmatrix.

|Die anzuschließende Diodenmatrix|
| ----------- |
|![](Pedalmatrix.png)|

Deswegen finden sich im Schaltplan unten 6 Stück 1k Widerstände, die in LMK3-typischer Art geschaltet sind. Die Ansteuerung des Pedals gelang auf Anhieb. Ängste, dass Ladungen aus den Ausgängen eventuell nicht schnell genug übertragen werden könnten, bevor deren Wirkungen über die Eingänge der Matrix erfragt würden, erwiesen sich als unbegründet. Gegen solche Effekte hätte es aber ein Mittel gegeben: verzögertes Auslesen durch die Software.

Das Pedal wird mit einem Sub-D Stecker an das Pianogehäuse angeschlossen.

|Digitalteil mit Arduino Mega 2560|
| ----------- |
|![](Arduino.PNG)|

Über den Anschluß von MIDI-Peripherie und LCD-Anzeigen an Arduino-Boards gibt es haufenweise Anleitungen. Diese brauchen hier nicht wiederholt zu werden. Der Mega 2560 könnte bis zu 4 MIDI-Ein- und Ausgänge bedienen. Ich habe die Schaltung auf 2 MIDI-Ein- und Ausgänge ausgelegt.

_Der Digitalteil wurde ab Herbst 2015 nach und nach in Betrieb genommen. Im Frühjahr 2016 zeigten sich nach längerem Betrieb erstmals lustige Effekte: Aussetzer, falsche Tonhöhen, ganz andere Klangfarben als die eingestellte. So hören sich typischerweise MIDI-Bitfehler an, und da es in kaltem Zustand nach dem Einschalten nie Fehler gab, war klar, dass es ein Temperatureffekt sein musste. Der 220 Ohm Widerstand am MIDI-Eingang war etwas zu groß bemessen für den verwendeten PC 817 Optokoppler. Die Reparatur erfolgte durch Austausch des PC 817 gegen ein anderes Exemplar. Alternativ hätte man den Widerstand verkleinern können._

Die Handräder werden zwar von relativ niederohmigen Potentiometern abgegriffen, sind allerdings über lange Leitungen mit der Steuerung verbunden. Die 470 nF Glättungskondensatoren sind deswegen tatsächlich erforderlich! Ohne sie bekäme die Software von der Hardware nerviges Gezappel.

Für die internen Taster und externen Schalter werden Pullup-Widerstände eingesetzt. Damit gab es noch nie ein falsches Signal.

Ein externer Anschluss ist ein Zwitter. Sowohl ein analoger (A7) als auch ein digitaler Eingang (51) teilen sich ihn.
Während sonst Arduino-interne Pullup-Widerstände zum Einsatz kommen, ist hier ein externer Pullup-Widerstand Rp = 68k verbaut.
Der Grund: es sollte ein vorhandenes Schwellerpedal angeschlossen werden. Das Schwellerpedal enthält ein 450k logarithmierendes Poti.
Die Software soll nicht linearisieren müssen. Das Poti liefert einen exponentiell vom Weg x abhängigen Widerstand R(x).
Durch den Spannungsteiler aus Pullup Rp und R ergibt sich eine Eingangsspannung U(R) ~ R/(R+Rp).
Durch geschickte Wahl von Rp kann man die verketteten nichtlinearen Funktionen R(x) und U(R)
gegeneinander ausspielen und so die Gesamtfunktion U(R(x)) = U(x) einigermaßen linear bekommen.
Die Entscheidung, ob der Anschluss für kontinuierliche oder diskrete MIDI Controller oder beide Arten zugleich verwendet wird, fällt erst in der Software.

2019 wurden ein Potentiometer und ein Schalter auf der rechten Seite der Frontblende hinzugefügt. Beide sind an analoge Eingänge angeschlossen, der Schalter mit einem 22k Pulldown Widerstand.

Es sind noch 3 analoge Eingänge unbeschaltet.

Das nächste Bild zeigt den Teil der Elektronik, der am aufklappbaren Deckel des Pianogehäuses hängt:

- links noch mit Ketron SD-2 Sound-Modul
- rechts Steuerung mit Arduino Board. Der Klinkenstecker kommt von den beiden Mono-Klinkenbuchsen für die externen Schalter. Der graue Stecker rechts führt zur Pedalanschlussbuchse (Sub-D).
- oben 2x20 zeilige LCD-Anzeige
- rechts neben dem Arduino pressspänerne Leere: später hat sich an diese Stelle der Audio-Mischer hinzugesellt.

|Elektronik im Deckel|
| ----------- |
|![](Elektronik_noch_ohne_Mischer.jpeg)|

Viel mehr ist zum Digitalteil nicht zu sagen, den Rest muss die Software erledigen.

## Audio-Mischer

Der Audiomischer hat zunächst ein eigenes Analognetzteil mit Transformator und Brückengleichrichter bekommen, um nicht auf unerwünschte Weise mit dem Digitalgedöns verbunden sein zu können (Brummschleife!). Ein 7809 Spannungsregler regelt auf 9V, was für viele Operationsverstärker ausreicht.

Es kommt ein Operationsverstärker TLE2072 zum Einsatz, der rauschärmer als ein TL072 sein soll.

|Schaltplan Audio-Mischer|
| ----------- |
|![](Mischer.PNG)|

Der Mischer "verstärkt" den für das MIDI-Modul vorgesehenen Stereo-Kanal mit dem Faktor 1. Der Lautstärkeregler des V3 ist immer voll aufgedreht. Die beiden externen Stereo-Kanäle werden mit einem Faktor 2 verstärkt. Das soll es erlauben, das interne Sound-Modul bei Bedarf zu übertönen.
Der Stereo-Ausgang ist nach außen geführt und auf den Eingang des GT-2 mini gelegt. Dort darf man das Eingangssignal nicht ganz aufdrehen, weil es sonst zu Übersteuerungen kommt.

|Der Mischer entsteht|
| ----------- |
|![](800px-Mischer_Versuchsaufbau.jpeg)|

Der Mischer wurde zunächst auf einem Experimentierbrett aufgebaut. Dabei gab es keine Überraschungen, außer dass ich in einem Anfall von geistiger Umnachtung zuerst den Spannungsregler nicht eingeplant hatte und mit entsprechendem Gebrumme bestraft wurde.

|Master-Slave-Switch und Mischer-Netzteil|
| ----------- |
|![](800px-Mischer_Analognetzteil.jpeg)|

Das Bild zeigt das graue Gehäuse des Master-Slave-Netzschalters und das daran angeschlossene Netzteil für den Mischer mit seinem blauen Transformator und 4 1N4006 Dioden in Brückenschaltung.

## Alles zusammen

|Elektronik untergebracht, noch ohne Mischer|
| ----------- |
|![](Unterbringung_Elektronik.jpg)|

|Mischer und neue Bedienelemente an der Frontblende|
| ----------- |
|![](Erweiterungen.png)|








