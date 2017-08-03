#pragma once
#include <vector>
#include "RandomChooser.h"
namespace algo {
	template<typename T>
	class MarkovChain {
	private:
		const int size;
		T* states;
		T state;
		util::RandomChooserFixed* chooser;
		float** matrix;
		int* entries;
		int _findIndexOfState(T s) {
			for (int i = 0; i < size; i++) {
				if (states[i] == s)
					return i;
			}
			return 0;
		}
		void _learnData(std::vector<T> data) {
			if (data.size() == 0) 
				return;
			T prev = data[0];
			state = data[0];
			typename std::vector<T>::iterator it;
			for (it = (++data.begin()); it != data.end(); ++it) {
				int prevI = _findIndexOfState(prev);
				int currI = _findIndexOfState(*it);
				matrix[prevI][currI]++;
				entries[prevI]++;
				prev = (*it);
			}
		}
		void _finishLearning() {
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++) {
					if (entries[i] == 0) continue;
					matrix[i][j] /= (float)entries[i];
				}
		}
	public:
		MarkovChain(int size) : size(size) {
			states = new T[size];
			for (int i = 0; i < size; i++)
				states[i] = 0;
			matrix = new float*[size];
			for (int i = 0; i < size; i++)
				matrix[i] = new float[size];
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					matrix[i][j] = 0;
			entries = new int[size];
			memset(entries, 0, sizeof(int) * size);
			chooser = new util::RandomChooserFixed(size);
		}
		~MarkovChain() {
			delete[] states;
			for (int i = 0; i < size; i++)
				delete[] matrix[i];
			delete[] matrix;
			delete[] entries;
		}
		void SetMatrix(float** ptr, int size) {
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++) {
					matrix[i][j] = ptr[i][j];
				}
		}
		void LearnDataSequence(std::vector<T>& data) {
			_learnData(data);
			_finishLearning();
		}
		void LearnDataSequence(std::vector<T>* data, int len) {
			for (int i = 0; i < len; i++) {
				_learnData(data[i]);
			}
			_finishLearning();
		}
		void LearnDataDuos(std::vector<T*>& data, int srcIndex, int dstIndex) {
			for (int i = 0; i < data.size(); i++) {
				int srcI = _findIndexOfState(data[i][srcIndex]);
				int dstI = _findIndexOfState(data[i][dstIndex]);
				matrix[srcI][dstI]++;
				entries[srcI]++;
			}
			_finishLearning();
		}
		void SetStateObject(int index, T newstate) {
			states[index] = newstate;
		}
		void SetState(T cstate) {
			state = cstate;
		}
		T GetState() {
			return state;
		}
		T SimulateNextWeighted() {
			int s = _findIndexOfState(state);
			chooser->SetChances(matrix[s]);
			int r = chooser->Choose();
			state = states[r];
			return state;
		}
		T SimulateNextUnweighted() {
			int s = _findIndexOfState(state);
			chooser->SetChances(matrix[s]);
			int r = chooser->ChooseUnweighted();
			state = states[r];
			return state;
		}
		void print() {
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					cout << matrix[i][j] << " , ";
				}
				cout << endl;
			}
		}
		void printLinks() {
			for (int i = 0; i < size; i++) {
				cout << i << " :: " << (int)states[i] << endl;
			}
		}
	};
};