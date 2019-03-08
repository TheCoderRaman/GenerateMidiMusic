#include "MIDIE.h"
#include "Algoritms.h"
#include "MusicalScales.h"
#include "MarkovChain.h"
#include <time.h>
#include <iomanip>
#include <iostream>
#include "MidiDataFilter.h"

#include "Midi/MidiFile.h"
#include "Midi/Options.h"
#include "MarkovChain.h"

MIDIE* midie = new MIDIE();
NoteTrack* track0 = new NoteTrack(0, 0);

void TestMidiEngine();

using namespace std;
int main(int argc, char** argv)
{
	std::cout << "Simple MIDI track by learned markovchain." << std::endl;
	std::cout << "Have a file called loadthismidi.mid in this directory." << std::endl;
	std::cout << "-Cody Bloemhard." << std::endl;
	//midi file loading stuff source:
	//https://github.com/craigsapp/midifile

	//uncomment to see basic functionality of MIDIE
	TestMidiEngine();

	MidiFile midifile;
	midifile.read("loadthismidi.mid");
	short tpq = midifile.getTicksPerQuarterNote();
	short millisPerQuarter = 500;
	cout << "TPQ: " << tpq << endl;
	int nChords = 0, uniqStartingChords = 0;

	int tracks = midifile.getTrackCount();
	std::vector<BYTE>* pitches = new std::vector<BYTE>[tracks];
	std::vector<BYTE>* volumes = new std::vector<BYTE>[tracks];
	std::vector<short>* intervals = new std::vector<short>[tracks];
	std::vector<BYTE*> chords;
	std::vector<BYTE> uniqChords;
	std::vector<BYTE> chordDegrees;
	std::vector<BYTE> uniqDegrees;

	for (int track = 0; track < tracks; track++) {
		FilterPitch_ON(pitches[track], midifile, track);
		FilterVolume_ON(volumes[track], midifile, track);
		FilterInterval_ON(intervals[track], midifile, track);
		FilterRange(intervals[track], (short)10, (short)1000);
	}
	FilterChords_ON(chords, 3, midifile);
	CountChordDegrees(chordDegrees, midifile);
	int uniqChordDegrees = FilterCountUniqueValues(chordDegrees, uniqDegrees);
	cout << "[ALL TRACKS LOADED]!" << endl;

	nChords = chords.size();
	uniqStartingChords = FilterCountChordsWithUniqueStart(chords, uniqChords);

	algo::MarkovChain<BYTE> pitchChain(128);
	for (int i = 0; i < 128; i++)
		pitchChain.SetStateObject(i, i);
	pitchChain.LearnDataSequence(pitches, tracks);
	
	algo::MarkovChain<BYTE> volumeChain(128);
	for (int i = 0; i < 128; i++)
		volumeChain.SetStateObject(i, i);
	volumeChain.LearnDataSequence(volumes, tracks);

	algo::MarkovChain<short> intervalChain(1000);
	for (int i = 0; i < 1000; i++)
		intervalChain.SetStateObject(i, i);
	intervalChain.LearnDataSequence(intervals, tracks);
	
	algo::MarkovChain<BYTE> chordChain(uniqStartingChords);
	for (int i = 0; i < uniqStartingChords; i++)
		chordChain.SetStateObject(i, uniqChords[i]);
	chordChain.LearnDataDuos(chords, 0, 1);

	algo::MarkovChain<BYTE> degreeChain(uniqChordDegrees);
	for (int i = 0; i < uniqChordDegrees; i++)
		degreeChain.SetStateObject(i, uniqDegrees[i]);
	degreeChain.LearnDataSequence(chordDegrees);
	cout << "[MUSIC LEARNED]" << endl;

	const int musiclen = 1000;
	for (int x = 0; x < musiclen; x++) {
		BYTE p = pitchChain.SimulateNextWeighted();
		BYTE v = volumeChain.SimulateNextWeighted();
		BYTE d = degreeChain.SimulateNextWeighted();
		chordChain.SetState(p);
		BYTE c0 = chordChain.SimulateNextWeighted();
		BYTE c1 = FindThirdNoteInChord(chords, p, c0);
		short i = intervalChain.SimulateNextWeighted();
		int notelen = (float)i / (float)tpq*millisPerQuarter;

		track0->AddLineRelative(notelen, Note(p, v));
		if (c0 != 0 && d >= 1) track0->AddLineRelative(0, Note(c0, v));
		if (c1 != 0 && d >= 2) track0->AddLineRelative(0, Note(c1, v));
		cout << (int)d << ", ";
	}

	midie->PlayNoteTrack(track0);
	system("PAUSE");
    return 0;
}

void TestMidiEngine() {
	NoteTrack* tr0 = new NoteTrack(0, 0);
	//Note(pitch, amplitude)
	//pitch and amplitude go from 0 to 128, so {0,1 ... 126,127}
	//add notes with absulute time(ms),
	tr0->AddLine(0, Note(25, 100));
	tr0->AddLine(250, Note(50, 100));
	tr0->AddLine(750, Note(75, 100));
	tr0->AddLine(1000, Note(127, 127));
	//add notes with relative time
	tr0->AddLineRelative(250, Note(30, 100));
	tr0->AddLineRelative(250, Note(35, 100));
	tr0->AddLineRelative(250, Note(40, 100));
	tr0->AddLineRelative(250, Note(45, 100));
	tr0->AddLineRelative(250, Note(50, 100));
	tr0->AddLineRelative(250, Note(55, 100));
	tr0->AddLineRelative(250, Note(60, 100));
	//play track
	midie->PlayNoteTrack(tr0);
	//multiple tracks
	NoteTrack* tr1 = new NoteTrack(0, INSTR_PIANO);
	NoteTrack* tr2 = new NoteTrack(1, INSTR_FRETLESS);
	for (int i = 0; i < 100; i++)
		tr1->AddLineRelative(200, Note(60 + (i % 10) * 5, 100));
	for (int i = 0; i < 100; i++)
		tr2->AddLineRelative(200, Note(40 + (i % 5) * 5, 100));
	Mix* mix = new Mix(midie);
	mix->AddTrack(tr1, 0.8f);
	mix->AddTrack(tr2, 1.0f);
	mix->Play();
	delete mix;
	delete tr0, tr1, tr2;
}