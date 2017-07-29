#pragma once
#include <vector>
#include <stdlib.h>
#include <ctime>

class RandomChooser {
private:
	std::vector<float> chances;
public:
	RandomChooser() {
		srand(time(NULL));
	}
	int uniformRand(int min, int max) {
		return ((double)rand() / ((double)RAND_MAX + 1.0)) * (max - min + 1) + min;
	}
	int Choose() {
		float bar = 0.0f;
		float r = ((float)(uniformRand(0, 10000))) / 10000.0f;
		int c = 0;
		std::vector<float>::iterator it;
		for (it = chances.begin(); it != chances.end(); ++it, c++) {
			float chuncksize = *it;
			if (bar + chuncksize >= r) {
				return c;
			}
			bar += chuncksize;
		}
	}
	void AddChance(float chance) {
		chances.push_back(chance);
	}
};