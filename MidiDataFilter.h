#pragma once
#include <vector>
#include "MIDIE.h"
#include "Midi/MidiFile.h"

std::vector<Note> FilterNotes_ON_AUDIBLE(MidiFile file, int track) {
	std::vector<Note> notes;
	for (int event = 0; event < file[track].size(); event++) {
		unsigned char cmd = file[track][event][0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (file[track][event][2] == 0) continue;
		Note n(file[track][event][1], file[track][event][2]);
		notes.push_back(n);
	}
	return notes;
}