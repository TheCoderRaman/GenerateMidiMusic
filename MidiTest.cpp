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

using namespace std;
int main(int argc, char** argv)
{
	std::cout << "Simple MIDI track by learned markovchain." << std::endl;
	std::cout << "-Cody Bloemhard." << std::endl;
	//https://github.com/craigsapp/midifile
	
	MidiFile midifile;
	midifile.read("beethoven_5e.mid");
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

	for (int track = 0; track < tracks; track++) {
		FilterPitch_ON(pitches[track], midifile, track);
		FilterVolume_ON(volumes[track], midifile, track);
		FilterInterval_ON(intervals[track], midifile, track);
		FilterRange(intervals[track], (short)10, (short)1000);
	}
	FilterChords_ON(chords, 3, midifile);
	cout << "[ALL TRACKS LOADED]!" << endl;

	/*std::vector<BYTE*>::iterator it;
	for (it = chords.begin(); it != chords.end(); ++it) {
		cout << (int)(*it)[0] << "," << (int)(*it)[1] << "," << (int)(*it)[2] << endl;
	}*/
	cout << (nChords = chords.size()) << endl;
	cout << (uniqStartingChords = FilterCountChordsWithUniqueStart(chords, uniqChords)) << endl;
	/*for (int i = 0; i < uniqStartingChords; i++)
		cout << (int)uniqChords[i] << " :: ";*/

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
	cout << "[MUSIC LEARNED]" << endl;

	const int musiclen = 1000;
	for (int x = 0; x < musiclen; x++) {
		BYTE p = pitchChain.SimulateNextWeighted();
		BYTE v = volumeChain.SimulateNextWeighted();
		chordChain.SetState(p);
		BYTE c0 = chordChain.SimulateNextWeighted();
		BYTE c1 = FindThirdNoteInChord(chords, p, c0);
		short i = intervalChain.SimulateNextWeighted();
		int notelen = (float)i / (float)tpq*millisPerQuarter;
		
		track0->AddLineRelative(notelen, Note(p, v));
		if (c0 != 0) track0->AddLineRelative(0, Note(c0, v));
		if (c1 != 0) track0->AddLineRelative(0, Note(c1, v));
	}

	midie->PlayNoteTrack(track0);
	system("PAUSE");
    return 0;
}