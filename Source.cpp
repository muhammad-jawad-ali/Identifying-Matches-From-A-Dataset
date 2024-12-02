#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
using namespace std;

class match {
public:
	int currentPlayerId;
	int currentTimestamp;
	vector<int> passIds;
	int keeperIdA;
	int keeperIdB;
	match(int cpi, int ct, vector<int> pi) {
		currentPlayerId = cpi;
		currentTimestamp = ct;
		passIds = pi;
	}
	match() {
		currentPlayerId = -1;
		currentTimestamp = -1;
	}
};


class pass {
public:
	int time_start;
	int time_end;
	int sender_id;
	int receiver_id;
	vector<int> x;
	vector<int> y;
	pass(int t_s, int t_e, int s_id, int r_id, vector<int> x, vector<int> y) {
		time_start = t_s;
		time_end = t_e;
		sender_id = s_id;
		receiver_id = r_id;
		this->x = x;
		this->y = y;
	}
	void print() {
		cout << "time_start: " << time_start << endl;
		cout << "time_end: " << time_end << endl;
		cout << "sender_id: " << sender_id << endl;
		cout << "receiver_id: " << receiver_id << endl;
		cout << "x: ";
		for (int i = 0; i < x.size(); i++) {
			cout << x[i] << " ";
		}
		cout << endl;
		cout << "y: ";
		for (int i = 0; i < y.size(); i++) {
			cout << y[i] << " ";
		}
		cout << endl;
	}

};



class CSVReader {
public:
	string fileName;
	char delimeter;
	vector<pass> passes;
	match* matches[14];
	int currentMatch = 0;
	CSVReader(string f, char d = ',') {
		fileName = f;
		delimeter = d;
		for (int i = 0; i < 14; i++){
			matches[i] = new match();
		}
		readData();
	}
	void readData() {
		ifstream file(fileName);
		cout << "Reading from the file: " << fileName << endl << endl;
		if (!file.is_open()) {
			cout << "File not found" << endl;
			return;
		}
		string line = "";

		//creat all variables
		int keeperIdsA[14];
		int keeperIdsB[14];
		int time_start;
		int time_end;
		int sender_id;
		int receiver_id;
		vector<int> x;
		vector<int> y;
		getline(file, line);
		while (getline(file, line)) {
			vector<string> vec;
			string word = "";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == delimeter) {
					vec.push_back(word);
					word = "";
				}
				else {
					word += line[i];
				}
			}
			vec.push_back(word);
			time_start = stoi(vec[0]);
			time_end = stoi(vec[1]);
			sender_id = stoi(vec[2]);
			receiver_id = stoi(vec[3]);
			vector<int> x;
			vector<int> y;
			for (int i = 4; i < 32; i++) {
				if (vec[i] != "")
					x.push_back(stoi(vec[i]));
				else
					x.push_back(99999);
			}
			for (int i = 32; i < 60; i++) {
				if (vec[i] != "")
					y.push_back(stoi(vec[i]));
				else
					y.push_back(99999);
			}
			passes.push_back(pass(time_start, time_end, sender_id, receiver_id, x, y));
			int keeperA = 0;
			int keeperB = 0;
			int maxpos = 0;
			int minpos = 0;
			for (int i = 0; i < x.size(); i++) {
				if (i == 0) {
					maxpos = x[i];
					minpos = x[i];
				}
				else {
					if (x[i] != 99999) {
						if (x[i] > maxpos) {
							maxpos = x[i];
							keeperA = i+1;
						}
						if (x[i] < minpos) {
							minpos = x[i];
							keeperB = i+1;
						}
					}
				}
			}
			cout << maxpos << " " << minpos << endl;
			cout << keeperA << " " << keeperB << endl;

			/*vector<int> pi;
			pi.push_back(passes.size() - 1);
			if (currentMatch == 0) {
				matches[0] = new match(receiver_id, time_end, pi);
				matches[0]->keeperIdA = keeperA;
				matches[0]->keeperIdB = keeperB;
				currentMatch++;
			}
			else {
				for (int i = 0; i < currentMatch; i++) {
					if (i < 14 && matches[i] != nullptr){
						if (matches[i]->currentPlayerId == sender_id && matches[i]->currentTimestamp <= time_start && (keeperA == matches[i]->keeperIdA || keeperB == matches[i]->keeperIdB)) {
							matches[i]->passIds.push_back(passes.size() - 1);
							matches[i]->currentTimestamp = time_end;
							matches[i]->currentPlayerId = receiver_id;
							break;
						}
						else if (i == currentMatch - 1) {
							matches[i] = new match(receiver_id, time_end, pi);
							matches[i]->keeperIdA = keeperA;
							matches[i]->keeperIdB = keeperB;
							currentMatch++;
							break;
						}
					}
				}

			}

		}
		cout << "Number of matches identified: ";
		cout << currentMatch << endl;*/
		file.close();
	}
};


int main() {
	CSVReader reader("first200.csv");
	return 0;
}