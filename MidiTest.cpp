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

MIDIE* midie = new MIDIE();
NoteTrack* track0 = new NoteTrack(0, 0);

using namespace std;
int main(int argc, char** argv)
{
	std::cout << "Simple MIDI track by learned markovchain." << std::endl;
	std::cout << "-Cody Bloemhard." << std::endl;
	//https://github.com/craigsapp/midifile
	
	MidiFile midifile;
	midifile.read("test.mid");
	
	int tracks = midifile.getTrackCount();
	std::vector<Note>* notes = new std::vector<Note>[tracks];

	for (int track = 0; track < tracks; track++) {
		cout << "Track " << track << endl;
		notes[track] = FilterNotes_ON_AUDIBLE(midifile, track);
	}

	std::vector<Note>::iterator it;
	for (it = notes[5].begin(); it != notes[5].end(); ++it) {
		std::cout << "[" << (int)it->hz << " , " << (int)it->db << "]\n";
	}

	system("PAUSE");
    return 0;
}