#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <list>
using namespace std;


class pass {
public:
	int time_start;
	int time_end;
	int sender_id;
	int receiver_id;
	pass(int t_s, int t_e, int s_id, int r_id) {
		time_start = t_s;
		time_end = t_e;
		sender_id = s_id;
		receiver_id = r_id;
	}
	void print() {
		cout << "time_start: " << time_start << ", ";
		cout << "time_end: " << time_end << ", ";
		cout << "sender_id: " << sender_id << ", ";
		cout << "receiver_id: " << receiver_id << endl;
	}

};

class match {
public:
	vector<pass> home_team_passes;
	vector<pass> away_team_passes;
	vector<int> substituteIds;
	int last_end_time;
	bool match_started;
	match() {
		last_end_time = -1;
		match_started = false;
	}

	void print() {
		cout << "Home team passes: " << endl;
		for (int i = 0; i < home_team_passes.size(); i++) {
			home_team_passes[i].print();
		}
		cout << "Away team passes: " << endl;
		for (int i = 0; i < away_team_passes.size(); i++) {
			away_team_passes[i].print();
		}
	}

	void insertPass(pass p) {
		if (p.sender_id >= 1 && p.sender_id <= 14 && p.receiver_id >= 1 && p.receiver_id <= 14) {
			home_team_passes.push_back(p);
		}
		else if (p.sender_id >= 15 && p.sender_id <= 28 && p.receiver_id >= 15 && p.receiver_id <= 28) {
			away_team_passes.push_back(p);
		}
	}

	void setsubstituteIds(vector<int> ids) {
		substituteIds = ids;
	}

	bool compare(vector<int> ids) {
		for (int i = 0; i < ids.size(); i++) {
			if (substituteIds[i] != ids[i]) {
				return false;
			}
		}
		return true;
	}
};



class CSVReader {
public:
	string fileName;
	char delimeter;
	list<match> matches;

	CSVReader(string f) {
		fileName = f;
		delimeter = ',';
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

		match m;
		matches.push_back(m);

		int time_start;
		int time_end;
		int sender_id;
		int receiver_id;

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
			for (int i = 4; i < 32; i++) {
				if (vec[i] == "")
					x.push_back(i - 3);
			}
			bool insert_success = false;
			for (auto& m : matches) {
				if (m.match_started == false) {
					m.insertPass(pass(time_start, time_end, sender_id, receiver_id));
					m.setsubstituteIds(x);
					m.last_end_time = time_end;
					m.match_started = true;
					insert_success = true;
					break;
				}
				else if (m.last_end_time <= time_start && m.compare(x)) {
					m.insertPass(pass(time_start, time_end, sender_id, receiver_id));
					m.last_end_time = time_end;
					insert_success = true;
					break;
				}
			}
			if (insert_success == false) {
				match m;
				m.insertPass(pass(time_start, time_end, sender_id, receiver_id));
				m.setsubstituteIds(x);
				m.last_end_time = time_end;
				m.match_started = true;
				matches.push_back(m);
			}
		}
		file.close();
	}

	void printMatchesDataSet() {
		int i = 1;
		for (auto& m : matches) {
			cout << "Match " << i << endl;
			m.print();
			i++;
			cout << endl << endl;
			char ch;
			cin >> ch;
		}
	}

	void generateSnapShots(int seconds, match m, string folder_TA, string folder_TB) {
		//adjacency matrix
		int snap_shot_graph[14][14] = { 0 };
		int reciever_id;
		int sender_id;
		int time_past = 0;
		int snap_shot_number = 1;
		for (int i = 0; i < m.home_team_passes.size(); i++) {
			reciever_id = m.home_team_passes[i].receiver_id;
			sender_id = m.home_team_passes[i].sender_id;
			time_past += m.home_team_passes[i].time_end;
			if (time_past <= seconds) {
				snap_shot_graph[sender_id - 1][reciever_id - 1]++;
			}
			else {
				string fileName = "home_team_snapshot_" + to_string(snap_shot_number) + ".txt";
				generateGraphProfile(snap_shot_graph, folder_TA + "/" + fileName);
				intializeArray(snap_shot_graph);
				snap_shot_graph[sender_id - 1][reciever_id - 1]++;
				time_past += m.home_team_passes[i].time_end;
				snap_shot_number++;
			}
		}
		snap_shot_number = 1;
		for(int i = 0; i < m.away_team_passes.size(); i++) {
			reciever_id = m.away_team_passes[i].receiver_id;
			sender_id = m.away_team_passes[i].sender_id;
			time_past += m.away_team_passes[i].time_end;
			if (time_past <= seconds) {
				snap_shot_graph[sender_id - 15][reciever_id - 15]++;
			}
			else {
				string fileName = "away_team_snapshot_" + to_string(snap_shot_number) + ".txt";
				generateGraphProfile(snap_shot_graph, folder_TB + "/" + fileName);
				intializeArray(snap_shot_graph);
				snap_shot_graph[sender_id - 15][reciever_id - 15]++;
				time_past += m.away_team_passes[i].time_end;
				snap_shot_number++;
			}
		}
	}

	void generateGraphProfile(int arr[14][14], string fileName) {
		ofstream file(fileName);
		string comment = "# number of Players\n";
		file << comment;
		file << "14\n";
		comment = "# Player Attributes\n";
		file << comment;
		for (int i = 0; i < 14; i++) {
			file << i << " ";
			comment = "Player " + to_string(i + 1) + "\n";
			file << comment;
		}
		for (int i = 0; i < 14; ++i) {
			comment = "# Pases By player " + to_string(i) + "\n";
			file << comment;
			int edges = 0;
			for (int j = 0; j < 14; j++) {
				if (arr[i][j] != 0) {
					edges++;
				}
			}
			file << edges << endl;
			if (edges != 0) {
				for (int j = 0; j < 14; j++) {
					if (arr[i][j] != 0) {
						file << i << " " << j << " " << arr[i][j] << endl;
					}
				}
			}
		}
		file.close();
		return;
	}

	void generateSnapShots(int seconds, int matchNumber, string folder_TA, string folder_TB) {
		int i = 1;
		for (auto& m : matches) {
			if (i == matchNumber) {
				generateSnapShots(seconds, m, folder_TA, folder_TB);
				break;
			}
			i++;
		}
	}

	void intializeArray(int arr[14][14]) {
		for (int i = 0; i < 14; i++) {
			for (int j = 0; j < 14; j++) {
				arr[i][j] = 0;
			}
		}
	}

};

int main() {
	CSVReader reader("passes_in_EXCEL_FORMAT.csv");
	//reader.printMatchesDataSet();
	string folderPath_TA, folderPath_TB;
	int matchNumber, timeWindow;
	cout<<"Enter the folder path to save snapshots (Team A): ";
	cin >> folderPath_TA;
	cout<<"Enter the folder path to save snapshots (Team B): ";
	cin >> folderPath_TB;
	cout<< "Enter the match number to genrate snapshots: ";
	cin >> matchNumber;
	cout<< "Enter the time window in milliseconds: ";
	cin >> timeWindow;
	reader.generateSnapShots(timeWindow, matchNumber, folderPath_TA,folderPath_TB);
	cout << "Program End!!!";
	return 0;
}