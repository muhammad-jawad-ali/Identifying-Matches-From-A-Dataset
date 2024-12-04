#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cstdlib>
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
	int last_start_time;
	bool match_started;
	int difference;
	int passes;

	match() {
		last_end_time = -1;
		match_started = false;
		difference = 0;
		last_start_time = 0;
		passes = 0;
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
			difference += (p.time_start - last_start_time);
			passes++;
			home_team_passes.push_back(p);
			last_start_time = p.time_start;
		}
		else if (p.sender_id >= 15 && p.sender_id <= 28 && p.receiver_id >= 15 && p.receiver_id <= 28) {
			difference += (p.time_start - last_start_time);
			passes++;
			away_team_passes.push_back(p);
			last_start_time = p.time_start;
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

	int getTimeWindow() {
		return difference / passes;
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
				else if (m.last_end_time <= time_start && m.compare(x)) {//&& sender_id == m.last_reciever ) {
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

	void generateSnapShots(int seconds, match m, string folder_TA, string folder_TB, int& ta_c, int& tb_c) {
		//adjacency matrix
		int snap_shot_graph[14][14] = { 0 };
		int reciever_id;
		int sender_id;
		int time_past = 0;
		int snap_shot_number = 1;
		for (int i = 0; i < m.home_team_passes.size(); i++) {
			reciever_id = m.home_team_passes[i].receiver_id;
			sender_id = m.home_team_passes[i].sender_id;
			time_past += (m.home_team_passes[i].time_end - m.home_team_passes[i].time_start);
			if (time_past <= seconds) {
				snap_shot_graph[sender_id - 1][reciever_id - 1]++;
			}
			else {
				time_past = 0;
				ta_c++;
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
			time_past += (m.away_team_passes[i].time_end - m.away_team_passes[i].time_start);
			if (time_past <= seconds) {
				snap_shot_graph[sender_id - 15][reciever_id - 15]++;
			}
			else {
				time_past = 0;
				tb_c++;
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
			comment = "\"A" + to_string(i + 1) + "\"\n";
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
						file << i << " " << j << endl;//" " << arr[i][j] << endl;
					}
				}
			}
		}
		file.close();
		return;
	}

	void generateSnapShots(int seconds, int matchNumber, string folder_TA, string folder_TB, string& snapshot_count_TA, string& snapshot_count_TB) {
		int i = 1;
		for (auto& m : matches) {
			if (i == matchNumber) {
				int ta_c = 0, tb_c = 0;
				generateSnapShots(seconds, m, folder_TA, folder_TB, ta_c, tb_c);
				snapshot_count_TA = to_string(ta_c);
				snapshot_count_TB = to_string(tb_c);
				break;
			}
			i++;
		}
	}

	int getRecommendedTimeWindow(int match) {
		int i = 1;
		for (auto& m : matches) {
			if (i == match) {
				return m.getTimeWindow();
			}
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
	string folderPath_TA = "A:/Matches/Match_1/home_team_graph", folderPath_TB = "A:/Matches/Match_1/away_team_graph", folderPath_PG = "A:/Matches/Match_1/pattern_folder_graph_";
	string home_team_snapshots_count, away_team_snapshots_count, pattern_graph_count = "1";
	int matchNumber = 2, timeWindow = 6000, patternGraphs = 8;
	cout<<"Enter the folder path to save snapshots (Team A): ";
	cin >> folderPath_TA;
	cout<<"Enter the folder path to save snapshots (Team B): ";
	cin >> folderPath_TB;
	cout<<"Enter the folder path to save snapshots (Pattern Graph): ";
	cin >> folderPath_PG;
	cout<< "Enter the match number to genrate snapshots: ";
	cin >> matchNumber;
	cout << "Enter the time window in milliseconds (recommended greater than: " << reader.getRecommendedTimeWindow(matchNumber) << " ): ";
	cin >> timeWindow;
	reader.generateSnapShots(timeWindow, matchNumber, folderPath_TA, folderPath_TB, home_team_snapshots_count, away_team_snapshots_count);
	cout << "Snapshots for Team A are saved in folder: " << folderPath_TA << " and total snapshots are " << home_team_snapshots_count << endl;
	cout << "Snapshots for Team B are saved in folder: " << folderPath_TB << " and total snapshots are " << away_team_snapshots_count << endl;
	string command;
	int patternGraphCode[9] = { 1, 12, 121, 123, 1213, 1231, 1232, 1234 };
	for (int i = 1; i <= patternGraphs; ++i) {
		cout << "Analysis of pattern graph " << i << " { " << patternGraphCode[i - 1] << " }:\n";
		cout << "Oberoi Analysis on Team A Snapshots:\n";
		command = "A:/FAST/Smester_3/DISCRETE_STRUCTURES/Project_Phase_1/x64/Debug/Project_Phase_1 " + folderPath_TA + " " + home_team_snapshots_count + " " + folderPath_PG + to_string(i) + " " + pattern_graph_count + " home_team_snapshot_ pattern_graph_";
		int result = system(command.c_str());
		if (result == 0) {
			cout << "Analysis Succesful\n" << endl;
		}
		cout << "Oberoi Analysis on Team B Snapshots:\n";
		command = "A:/FAST/Smester_3/DISCRETE_STRUCTURES/Project_Phase_1/x64/Debug/Project_Phase_1 " + folderPath_TB + " " + away_team_snapshots_count + " " + folderPath_PG + to_string(i) + " " + pattern_graph_count + " away_team_snapshot_ pattern_graph_";
		result = system(command.c_str());
		if (result == 0) {
			cout << "Analysis Succesful\n" << endl;
		}
		char ch;
		cin >> ch;
	}
	int a = 0, b = 1;
	pattern_graph_count = "2";
	for (int i = 10; i <= 24; ++i) {
		cout << "Analysis of pattern graph " << i << " { " << patternGraphCode[a] << "->" << patternGraphCode[b] << " }:\n";
		cout << "Oberoi Analysis on Team A Snapshots:\n";
		command = "A:/FAST/Smester_3/DISCRETE_STRUCTURES/Project_Phase_1/x64/Debug/Project_Phase_1 " + folderPath_TA + " " + home_team_snapshots_count + " " + folderPath_PG + to_string(i) + " " + pattern_graph_count + " home_team_snapshot_ pattern_graph_";
		int result = system(command.c_str());
		if(result == 0) {
			cout << "Analysis Succesful\n" << endl;
		}
		cout << "Oberoi Analysis on Team B Snapshots:\n";
		command = "A:/FAST/Smester_3/DISCRETE_STRUCTURES/Project_Phase_1/x64/Debug/Project_Phase_1 " + folderPath_TB + " " + away_team_snapshots_count + " " + folderPath_PG + to_string(i) + " " + pattern_graph_count + " away_team_snapshot_ pattern_graph_";
		result = system(command.c_str());
		if (result == 0) {
			cout << "Analysis Succesful\n" << endl;
		}
		char ch;
		cin >> ch;
		if(b == 3) {
			a++;
			b = 0;
		}
		else {
			b++;
		}
	}

	cout << "Program End!!!";
	
	return 0;
}