#pragma once
#include <vector>
#include <stdlib.h>
#include <ctime>
namespace util {
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
	class RandomChooserFixed {
	private:
		float* chances;
		int size;
	public:
		RandomChooserFixed(int size) {
			srand(time(NULL));
			chances = new float[size];
			this->size = size;
		}
		int uniformRand(int min, int max) {
			return ((double)rand() / ((double)RAND_MAX + 1.0)) * (max - min + 1) + min;
		}
		int Choose() {
			float bar = 0.0f;
			float r = ((float)(uniformRand(0, 10000))) / 10000.0f;
			int c = 0;
			for (int i = 0; i < size; i++, c++) {
				float chuncksize = chances[i];
				if (bar + chuncksize >= r) {
					return c;
				}
				bar += chuncksize;
			}
		}
		int ChooseUnweighted() {
			int ss = 0;
			int validchoices[6];
			for (int i = 0; i < size; i++) {
				if (chances[i] != 0.0f) {
					validchoices[ss] = i;
					ss++;
				}
			}
			float r = ((float)(uniformRand(0, 10000 * ss))) / 10000.0f;
			int temp = (int)r;
			return validchoices[temp];
		}
		void SetChances(float values[]) {
			chances = values;
		}
	};
};