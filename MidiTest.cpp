#include "MIDIE.h"
#include "Algoritms.h"
#include <time.h>

MIDIE* midie = new MIDIE();
NoteTrack* track0 = new NoteTrack(0, 0);
NoteTrack* track1 = new NoteTrack(CHL_DRUM, 0);
NoteTrack* track2 = new NoteTrack(1, INSTR_FRETLESS);

int main()
{
	std::cout << "Simple MIDI track using Collatz conj. and Primegaps." << std::endl;
	std::cout << "-Cody Bloemhard." << std::endl;

	//Collatz conjecture and Primegaps
	int col_tone = 95, col_len = 500;
	BYTE res = 60;
	int prime = 13;
	for (int i = 0; i < 100; i++) {
		if (colatz(col_tone)) res -= 6;
		else res += 9;

		col_len = nextPrimegap(prime);
		prime += col_len;

		if(col_len <= 6)
			for (int i = 0; i < 6 - col_len; i++)
				track0->AddLineRelative(col_len*50, Note(res + i, 100));
		else track0->AddLineRelative(col_len * 100, Note(res, 100));
	}
	track0->End(1000);
	//generate beat
	prime = 13;
	for (int i = 0; i < 100; i++) {
		col_len = nextPrimegap(prime);
		prime += col_len;
		if (col_len <= 8) {
			for (int j = 0; j < col_len; j++) {
				track1->AddLineRelative((1.0/col_len)*1000, Note(35, 100));
			}
		}
	}
	track1->End(1000);
	//bass line??
	prime = 13;
	BYTE bassPitch = 0;
	for (int i = 0; i < 100; i++) {
		col_len = nextPrimegap(prime);
		prime += col_len;

		bassPitch = col_len*10;

		if (col_len <= 5) {
			for (int j = 0; j < 5 - col_len; j++) {
				track2->AddLineRelative((1.0 / col_len) * 1000, Note(bassPitch, 100));
			}
		}
	}
	track2->End(1000);
	
	Mix* mix = new Mix(midie);
	mix->AddTrack(track0, 1.0f);
	mix->AddTrack(track1, 0.8f);
	mix->AddTrack(track2, 1.0f);
	mix->Play();

	system("PAUSE");
    return 0;
}