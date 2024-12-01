#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
using namespace std;


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
	CSVReader(string f, char d = ',') {
		fileName = f;
		delimeter = d;
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
		}
		cout << "Number of passes: " << passes.size() << endl;
		passes[0].print();
		file.close();
	}
};


int main() {
	CSVReader reader("passes_in_EXCEL_FORMAT.csv");
	return 0;
}