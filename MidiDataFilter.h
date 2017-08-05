#pragma once
#include <vector>
#include "MIDIE.h"
#include "Midi/MidiFile.h"
#include <algorithm>
#include "RandomChooser.h"

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

template <typename T>
int CountUniqueValues(std::vector<T>& vec) {
	int c = 0;
	T* counter = new T[vec.size()];
	typename std::vector<T>::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		bool newv = true;
		for (int i = 0; i < c; i++) {
			if (*it == counter[i]) {
				newv = false;
				break;
			}
		}
		if (newv) {
			counter[c] = *it;
			c++;
		}
	}

	delete[] counter;
	return c;
}

template <typename T>
int FilterCountUniqueValues(std::vector<T>& data, std::vector<T>& vec) {
	int c = 0;
	T* counter = new T[data.size()];
	typename std::vector<T>::iterator it;
	for (it = data.begin(); it != data.end(); ++it) {
		bool newv = true;
		for (int i = 0; i < c; i++) {
			if (*it == counter[i]) {
				newv = false;
				break;
			}
		}
		if (newv) {
			counter[c] = *it;
			vec.push_back(*it);
			c++;
		}
	}

	delete[] counter;
	return c;
}

void FilterNotes_ON(std::vector<Note>& vec, MidiFile file, int track) {
	for (int event = 0; event < file[track].size(); event++) {
		unsigned char cmd = file[track][event][0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (file[track][event][2] == 0) continue;
		Note n(file[track][event][1], file[track][event][2]);
		vec.push_back(n);
	}
}

void FilterPitch_ON(std::vector<BYTE>& vec, MidiFile file, int track) {
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[1] == 0 || e[2] == 0) continue;
		vec.push_back(e[1]);
	}
}

void FilterVolume_ON(std::vector<BYTE>& vec, MidiFile file, int track) {
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[1] == 0 || e[2] == 0) continue;
		vec.push_back(e[2]);
	}
}

void FilterInterval_ON(std::vector<short>& vec, MidiFile file, int track) {
	short prev = 0;
	MidiEventList& list = file[track];
	for (int event = 0; event < list.size(); event++) {
		MidiEvent e = list[event];
		unsigned char cmd = e[0];
		if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
		if (e[1] == 0 || e[2] == 0) continue;
		short delta = e.tick - prev;
		prev = e.tick;
		if (delta <= 0) continue;
		vec.push_back(delta);
	}
}

bool SortMidiEventByTick(const MidiEvent& l, const MidiEvent& r) {
	return l.tick < r.tick;
}

bool SortChordByFirstNote(const BYTE* l, const BYTE* r) {
	return l[0] < r[0];
}

void FilterChords_ON(std::vector<BYTE*>& vec, int arraysize, MidiFile file) {
	short prev = 0;
	int c = 0;
	BYTE* temparr = new BYTE[arraysize];
	std::vector<MidiEvent> allevents;
	for (int i = 0; i < file.getTrackCount(); i++) {
		for (int j = 0; j < file[i].size(); j++) {
			MidiEvent e = file[i][j];
			unsigned char cmd = e[0];
			if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
			if (e[1] == 0 || e[2] == 0) continue;
			allevents.push_back(e);
		}
	}
	sort(allevents.begin(), allevents.end(), SortMidiEventByTick);
	for (int i = 0; i < allevents.size(); i++) {
		MidiEvent e = allevents[i];
		short delta = e.tick - prev;
		if (delta == 0 && c < arraysize) {
			temparr[c++] = e[1];//:D
		}
		else {
			if (c > 0) {
				vec.push_back(temparr);
				temparr = new BYTE[arraysize];
			}
			c = 0;		
		}
		prev = e.tick;
	}
	sort(vec.begin(), vec.end(), SortChordByFirstNote);
}

int FilterCountChordsWithUniqueStart(std::vector<BYTE*>& data, std::vector<BYTE>& uniqChords) {
	int c = 0;
	BYTE prevnote = 0;
	std::vector<BYTE*>::iterator it;
	for (it = data.begin(); it != data.end(); ++it) {
		if (*it[0] != prevnote) {
			c++;//:D
			prevnote = *it[0];
			uniqChords.push_back(*it[0]);
		}
	}
	return c;
}

int FindThirdNoteInChord(const std::vector<BYTE*>& vec, int fn, int sn) {
	int index = 0;
	int possibilties = 0;
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i][0] == fn) {
			index = i;
			break;
		}
	}
	for (int i = index; i < vec.size() - index; i++) {
		if (vec[i][2] == sn) {
			index = i;
			break;
		}
	}
	for (int i = index; i < vec.size() - index; i++) {
		if (vec[i][2] == sn) {
			possibilties++;
		}
		else {
			break;
		}
	}
	float r = (float)util::RandomChooser::uniformRand(0, 10000 * possibilties) / 10000.0f;
	for (int i = 0; i < possibilties; i++) {
		if (r < i)
			return index + i;
	}
}

int* CountChordDegrees(std::vector<BYTE>& vec, MidiFile file) {
	int* degrees = new int[4]{0};
	short prev = 0;
	int c = 0;
	std::vector<MidiEvent> allevents;
	for (int i = 0; i < file.getTrackCount(); i++) {
		for (int j = 0; j < file[i].size(); j++) {
			MidiEvent e = file[i][j];
			unsigned char cmd = e[0];
			if (!(cmd >= 0x90 && cmd <= 0x90 + 16)) continue;
			if (e[1] == 0 || e[2] == 0) continue;
			allevents.push_back(e);
		}
	}
	sort(allevents.begin(), allevents.end(), SortMidiEventByTick);
	for (int i = 0; i < allevents.size(); i++) {
		MidiEvent e = allevents[i];
		short delta = e.tick - prev;
		if (delta == 0) {
			c++;//:D
		}
		else {
			vec.push_back(c);
			c = 0;
		}
		prev = e.tick;
	}
	return degrees;
}