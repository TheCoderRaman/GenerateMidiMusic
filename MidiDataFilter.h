#pragma once
#include <vector>
#include "MIDIE.h"
#include "Midi/MidiFile.h"

void FilterNotes_ON_AUDIBLE(std::vector<Note>& vec, MidiFile file, int track) {
	for (int event = 0; event < file[track].size(); event++) {
		unsigned char cmd = file[track][event][0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (file[track][event][2] == 0) continue;
		Note n(file[track][event][1], file[track][event][2]);
		vec.push_back(n);
	}
}

void FilterPitch_ON_AUDIBLE(std::vector<BYTE>& vec, MidiFile file, int track) {
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[2] == 0) continue;
		vec.push_back(e[1]);
	}
}

void FilterVolume_ON_AUDIBLE(std::vector<BYTE>& vec, MidiFile file, int track) {
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[2] == 0) continue;
		vec.push_back(e[2]);
	}
}

void FilterInterval_ON_AUDIBLE(std::vector<short>& vec, MidiFile file, int track) {
	short prev = 0;
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[2] == 0) continue;
		short delta = e.tick - prev;
		prev = e.tick;
		if (delta <= 0) continue;
		vec.push_back(delta);
	}
}

template <typename T>
void FilterRange(std::vector<T>& vec, T min, T max) {
	std::vector<T> newvec;
	typename std::vector<T>::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		if (*it <= max && *it >= min)
			newvec.push_back(*it);
	}
	vec.clear();
	vec = newvec;
}