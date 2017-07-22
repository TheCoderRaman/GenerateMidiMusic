#pragma once
#define BYTE unsigned char
#define NOTE_C 0
#define NOTE_CS 1
#define NOTE_D 2
#define NOTE_DS 3
#define NOTE_E 4
#define NOTE_F 5
#define NOTE_FS 6
#define NOTE_G 7
#define NOTE_GS 8
#define NOTE_A 9
#define NOTE_AS 10
#define NOTE_B 11

#define TONE 2
#define SEMI 1

int NOTES[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
std::string NOTENAMES[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
BYTE MAJOR_PROGRESSION[7] = { TONE, TONE, SEMI, TONE, TONE, TONE, SEMI };
BYTE MINOR_PROGRESSION[7] = { TONE, SEMI, TONE, TONE, SEMI, TONE, TONE };

struct MusicalNote {
	BYTE letter;
	char octave;
	BYTE midiPitch;
	MusicalNote() {
		letter = 0;
		octave = 0;
		midiPitch = 0;
	}
	MusicalNote(BYTE letter, char octave) {
		this->letter = letter;
		this->octave = octave;
		this->midiPitch = letter + (12 * (octave+1));
	}
	MusicalNote(BYTE midiPitch) {
		this->midiPitch = midiPitch;
		letter = midiPitch;
		octave = -1;
		while (letter > 11) {
			letter -= 12;
			octave++;
		}
	}
	void UpdateLetter() {
		letter = midiPitch;
		octave = -1;
		while (letter > 11) {
			letter -= 12;
			octave++;
		}
	}
	void UpdateMidi() {
		midiPitch = letter + (12 * (octave + 1));
	}
	std::string ToString(bool letters) {
		if (letters) return (NOTENAMES[letter]).append(std::to_string(octave));
		else return std::to_string(midiPitch);
	}
};

namespace Scales {
	struct HeptatonicScale {
		MusicalNote notes[7];
		int midis[7];
		HeptatonicScale() {}
		std::string ToString() {
			std::string s;
			for (int i = 0; i < 7; i++) {
				s.append((NOTENAMES[notes[i].letter])
					.append(std::to_string(notes[i].octave))).append(" - ");
			}
			return s;
		}
	};
	void ChromaticProgressionLetter(BYTE progression, BYTE& note, char& octave) {
		note += progression;
		if (note > 11) {
			octave++;
			note -= 12;
		}
	}
	void ChromaticProgressionMidi(BYTE progression, BYTE& pitch) {
		pitch += progression;
		if (pitch > 127)
			pitch -= 127;
	}
	HeptatonicScale Major(BYTE startingNote, char startingOctave) {
		HeptatonicScale s;
		BYTE note = startingNote;
		char octave = startingOctave;
		for (int i = 0; i < 7; i++) {
			s.notes[i].letter = note;
			s.notes[i].octave = octave;
			s.notes[i].UpdateMidi();
			s.midis[i] = s.notes[i].midiPitch;
			ChromaticProgressionLetter(MAJOR_PROGRESSION[i], note, octave);
		}
		return s;
	}
	HeptatonicScale Minor(BYTE startingNote, char startingOctave) {
		HeptatonicScale s;
		BYTE note = startingNote;
		char octave = startingOctave;
		for (int i = 0; i < 7; i++) {
			s.notes[i].letter = note;
			s.notes[i].octave = octave;
			s.notes[i].UpdateMidi();
			s.midis[i] = s.notes[i].midiPitch;
			ChromaticProgressionLetter(MINOR_PROGRESSION[i], note, octave);
		}
		return s;
	}
};