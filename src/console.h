/*
 * console.h
 *
 *  Created on: 17 paü 2015
 *      Author: Grzegurz
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

enum Command {
	speed = 0,
	go = 1,
	light_ambient = 2,
	light_diffuse = 3,
	light_specular = 4,
	light_position = 5,
	range = 6,
	LOD = 7,
	edit = 8,
	end_edit = 9,
	quit = 10
};

class Action;

class Console {
private:
	int mainLine;
	const string LIGHT_ERROR = "Invalid light args";

	void nextLine() {
		for (int i = 1; i < lineNumber; i++) {
			lines[i - 1] = lines[i];
		}
		lines[lineNumber - 1] = "";
		pointer = 0;
	}

	vector<string> split(string s) {
		istringstream iss(s);
		vector < string > tokens;
		copy(istream_iterator < string > (iss), istream_iterator<string>(), back_inserter(tokens));
		return tokens;
	}

	void parse();

	void init();

public:
	map<string, Action*> commands;
	string* lines;
	bool typing = false;
	int lineNumber;
	int pointer = 0;

	Console(int lineNumber) {
		this->lineNumber = lineNumber;
		mainLine = lineNumber - 1;
		lines = new string[lineNumber];
		for (int i = 0; i < lineNumber; i++) {
			lines[i] = "";
		}
		init();
	}

	void type(string str) {
		vector < string > result;
		stringstream ss(str);
		string item;
		while (getline(ss, item, '\n')) {
			result.push_back(item);
		}
		for (unsigned i = 0; i < result.size(); i++) {
			nextLine();
			lines[mainLine - 1] = result[i];
		}
	}

	void clear() {
		for (int i = 0; i < lineNumber; i++) {
			nextLine();
		}
	}
	void type(char c) {
		switch (c) {
		case 13:
			parse();
			nextLine();
			typing = false;
			break;
		case 27:
			typing = false;
			lines[mainLine] = "";
			pointer = 0;
			break;
		case '\b': {
			if (pointer > 0) {
				pointer--;
			}
			lines[mainLine].erase(pointer, 1);
			break;
		}
		case 127: //delete
			lines[mainLine].erase(pointer, 1);
			break;
		default:
			string a(1, c);
			lines[mainLine].insert(pointer, a);
			pointer++;
		}
	}

	void typeSpecial(char c) {
		switch (c) {
		case GLUT_KEY_LEFT:
			if (pointer > 0) {
				pointer--;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (pointer < (int) lines[lineNumber - 1].size() - 1) {
				pointer++;
			}
			break;
		}
	}
};

#endif /* SRC_CONSOLE_H_ */
