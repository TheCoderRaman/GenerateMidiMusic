#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <vector>
#include <bitset>
#include <thread>

#define MD 0
#define HZ 1
#define DB 2
#define CH 3

#define MSG_NOTE_ON 0x90
#define MSG_NOTE_OFF 0x80
#define MSG_INSTRUMENT 0xc0
#define MSG_DRUM 0x99		//== 0x90 | 9
#define CHL_DRUM 9

#define INSTR_PIANO 0
#define INSTR_VIOLIN 41
#define INSTR_FRETLESS 36

#define DRM_KICK 0

#define BYTE unsigned char
#define ULONG unsigned long
#define MESSAGE union { ULONG w; BYTE d[4]; }

struct Note {
	BYTE hz, db;
	Note() {}
	Note(BYTE hz, BYTE db) {
		this->hz = hz;
		this->db = db;
	}
};

struct Line {
	short t;
	Note n;
	Line(){}
	Line(short tt, Note nn) {
		t = tt;
		n = nn;
	}
	Line(short tt, BYTE hz, BYTE db) {
		t = tt;
		n = Note(hz, db);
	}
};

class NoteTrack {
private:
	short time = 0;
	float volume;
public:
	std::vector<Line> track;
	BYTE channel;
	BYTE instrument;
	NoteTrack(BYTE channel, BYTE instrument) {
		this->channel = channel;
		this->instrument = instrument;
		volume = 1.0f;
	}
	void AddLine(Line l) {
		track.push_back(l);
		time = l.t;
	}
	void Addline(short t, Note n) {
		track.push_back(Line(t, n));
		time = t;
	}
	void Addline(short t, BYTE hz, BYTE db) {
		track.push_back(Line(t, hz, db));
		time = t;
	}
	void AddLineRelative(short t, Note n) {
		track.push_back(Line(time + t, n));
		time += t;
	}
	void AddLineRelative(short t, BYTE hz, BYTE db) {
		track.push_back(Line(time + t, hz, db));
		time += t;
	}
	void End(short t) {
		track.push_back(Line(time + t, 0, 0));
	}
	void SetVolume(float v) {
		volume = v;
	}
	float GetVolume() {
		return volume;
	}
};

class MIDIE {
private:
	int midiport, flag;
	HMIDIOUT device;
	MESSAGE m;
	BYTE channel = 0;
	float mv = 1.0f;
public:
	MIDIE() {
		midiport = 0;
		flag = midiOutOpen(&device, midiport, 0, 0, CALLBACK_NULL);
		if (flag != MMSYSERR_NOERROR) 
			std::cout << "[ERROR] MIDI output could not be opened.\n";
	}
	void Wait(short t) {
		Sleep(t);
	}
	void PlayNote(BYTE hz, BYTE db) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = hz;
		m.d[DB] = db * mv;
		flag = midiOutShortMsg(device, m.w);
	}
	void EndNote(BYTE hz) {
		m.d[MD] = MSG_NOTE_OFF | channel;
		m.d[HZ] = hz;
		m.d[DB] = 0;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayNoteChannel(BYTE hz, BYTE db, BYTE ch) {
		m.d[MD] = MSG_NOTE_ON | ch;
		m.d[HZ] = hz;
		m.d[DB] = db * mv;
		flag = midiOutShortMsg(device, m.w);
	}
	void EndNoteChannel(BYTE hz, BYTE ch) {
		m.d[MD] = MSG_NOTE_OFF | ch;
		m.d[HZ] = hz;
		m.d[DB] = 0;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayNote(Note n) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = n.hz;
		m.d[DB] = n.db * mv;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayNoteBlocked(BYTE hz, BYTE db, short t) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = hz;
		m.d[DB] = db * mv;
		flag = midiOutShortMsg(device, m.w);
		Sleep(t);
	}
	void PlayNoteBlocked(Note n, short t) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = n.hz;
		m.d[DB] = n.db * mv;
		flag = midiOutShortMsg(device, m.w);
		Sleep(t);
	}
	void PlayNoteAutoBlocked(BYTE hz, BYTE db, short t) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = hz;
		m.d[DB] = db * mv;
		flag = midiOutShortMsg(device, m.w);
		Sleep(t);
		m.d[MD] = MSG_NOTE_OFF | channel;
		m.d[HZ] = hz;
		m.d[DB] = 0;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayNoteAutoBlocked(Note n, short t) {
		m.d[MD] = MSG_NOTE_ON | channel;
		m.d[HZ] = n.hz;
		m.d[DB] = n.db * mv;
		flag = midiOutShortMsg(device, m.w);
		Sleep(t);
		m.d[MD] = MSG_NOTE_OFF | channel;
		m.d[HZ] = n.hz;
		m.d[DB] = 0;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayDrum(BYTE type, BYTE db) {
		m.d[MD] = MSG_DRUM;//== MSG_NOTE_ON | 9
		m.d[HZ] = type;
		m.d[DB] = db * mv;
		flag = midiOutShortMsg(device, m.w);
	}
	void PlayNoteTrack(NoteTrack* t) {
		m.d[MD] = MSG_INSTRUMENT | t->channel;
		m.d[HZ] = t->instrument;
		flag = midiOutShortMsg(device, m.w);
		BYTE modeToUse;
		if (t->channel == 9)//drum channel
			modeToUse = MSG_DRUM;
		else
			modeToUse = MSG_NOTE_ON | t->channel;
		short time = 0, timer = 0;
		std::vector<Line>::iterator it;
		for (it = t->track.begin();  it != t->track.end(); it++) {
			timer = it->t - time;
			Sleep(timer);
			time += timer;
			m.d[MD] = modeToUse;
			m.d[HZ] = it->n.hz;
			m.d[DB] = it->n.db * t->GetVolume() * mv;
			flag = midiOutShortMsg(device, m.w);
		}
	}
	float GetMasterVolume() {
		return mv;
	}
	void SetMasterVolume(float v) {
		mv = v;
	}
	void SetInstrument(BYTE instrument, BYTE channel) {
		m.d[MD] = MSG_INSTRUMENT | channel;
		m.d[HZ] = instrument;
		flag = midiOutShortMsg(device, m.w);
	}
	void SetChannel(BYTE channel) {
		this->channel = channel;
	}
	void Send(unsigned long word) {
		flag = midiOutShortMsg(device, word);
	}
	void Close() {
		midiOutReset(device);
		midiOutClose(device);
	}
};

class Fctor {
public:
	void operator()(MIDIE* m, NoteTrack* t) {
		m->PlayNoteTrack(t);
	}
};

class Mix {
private:
	float masterVolume;
	std::vector<NoteTrack*> tracks;
	MIDIE* midie;
public:
	Mix(MIDIE* m) {
		midie = m;
	}
	void SetMasterVolume(float v) {
		masterVolume = v;
		midie->SetMasterVolume(v);
	}
	void AddTrack(NoteTrack* t, float v) {
		t->SetVolume(v);
		tracks.push_back(t);
	}
	void Play() {
		std::vector<std::thread> threads;
		std::vector<NoteTrack*>::iterator track_it;
		for (track_it = tracks.begin(); track_it != tracks.end(); track_it++) {
			Fctor f;
			threads.push_back(std::thread(f, midie, (*track_it)));
		}
		std::vector<std::thread>::iterator thread_it;
		for (thread_it = threads.begin(); thread_it != threads.end(); thread_it++) {
			(*thread_it).join();
		}
	}
};