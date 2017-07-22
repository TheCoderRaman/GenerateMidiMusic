#include "MIDIE.h"
#include "Algoritms.h"
#include "MusicalScales.h"
#include "MarkovChain.h"
#include <time.h>

MIDIE* midie = new MIDIE();
NoteTrack* track0 = new NoteTrack(0, 0);
NoteTrack* track1 = new NoteTrack(CHL_DRUM, 0);
NoteTrack* track2 = new NoteTrack(1, INSTR_FRETLESS);

int main()
{
	std::cout << "Simple MIDI track using Collatz conj. and Primegaps." << std::endl;
	std::cout << "-Cody Bloemhard." << std::endl;

	Scales::HeptatonicScale Dmajor = Scales::Major(NOTE_D, 4);
	Scales::HeptatonicScale Dminor = Scales::Major(NOTE_D, 4);
	int pos = 0;

	RandomChooser chooser;
	int results[10] = {0,0,0,0,0,0,0,0,0,0};
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	chooser.AddChance(0.10);
	for (int i = 0; i < 1000000; i++) {
		results[chooser.Choose()]++;
	}
	for (int i = 0; i < 10; i++) {
		std::cout << "["<< i << "]: " << (float)results[i] / (float)1000000 << std::endl;
	}

	MarkovChain chain;
	chain.AddNode('A');
	chain.AddNode('B');
	chain.AddLink('A', 'A', 0.7f);
	chain.AddLink('A', 'B', 0.3f);
	chain.AddLink('B', 'B', 0.5f);
	chain.AddLink('B', 'A', 0.5f);
	chain.InitChain();
	for (int i = 0; i < 100; i++){
		char prev = chain.GetState();
		chain.AdvanceState();
		if (chain.GetState() != prev) {
			pos = 0;
		}
		if (chain.GetState() == 'A') {
			track0->AddLineRelative(250, Note(Dmajor.midis[pos], 100));
			pos++;
		}
		else {
			track0->AddLineRelative(250, Note(Dminor.midis[pos], 100));
			pos++;
		}
	}

	Mix* mix = new Mix(midie);
	mix->AddTrack(track0, 1.0f);
	mix->AddTrack(track1, 0.8f);
	mix->AddTrack(track2, 1.0f);
	mix->SetMasterVolume(1.0f);
	mix->Play();
	system("PAUSE");
    return 0;
}