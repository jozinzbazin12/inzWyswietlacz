/*
 * console.h
 *
 *  Created on: 17 paü 2015
 *      Author: Grzegurz
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include <string>

enum Command {
	speed = 0, go = 1, light_ambient = 2, light_diffuse = 3, light_specular = 4, light_position = 5, position = 6, range = 7
};

class Console {
private:
	int actualLine = 0;
	map<string, Command> commands;

	void nextLine() {
		for (int i = 1; i < lineNumber; i++) {
			lines[i - 1] = lines[i];
		}
		lines[lineNumber - 1] = "";
		actualLine = (actualLine + 1) % lineNumber;
		pointer = 0;
	}

	vector<string> split(string s) {
		istringstream iss(s);
		vector < string > tokens;
		copy(istream_iterator < string > (iss), istream_iterator<string>(), back_inserter(tokens));
		return tokens;
	}

	void parse() {
		vector < string > result = split(lines[lineNumber - 1]);
		Command command = commands.find(result[0])->second;
		switch (command) {
		case speed:
			if (result.size() >= 2) {
				predkosc = stod(result[1]);
			}
			break;
		case go:
			if (result.size() >= 4) {
				posX = stod(result[1]);
				posY = stod(result[2]);
				posZ = stod(result[3]);
			}
			break;
		case light_ambient:
			if (result.size() >= 5) {
				Light::getInstance()->setAmbient(stod(result[1]), stod(result[2]), stod(result[3]), stod(result[4]));
				Light::getInstance()->commit();
			}
			break;
		case light_diffuse:
			if (result.size() >= 5) {
				Light::getInstance()->setDiffuse(stod(result[1]), stod(result[2]), stod(result[3]), stod(result[4]));
				Light::getInstance()->commit();
			}
			break;
		case light_specular:
			if (result.size() >= 5) {
				Light::getInstance()->setSpecular(stod(result[1]), stod(result[2]), stod(result[3]), stod(result[4]));
				Light::getInstance()->commit();
			}
			break;
		case light_position:
			if (result.size() >= 5) {
				Light::getInstance()->setPosition(stod(result[1]), stod(result[2]), stod(result[3]), stod(result[4]));
				Light::getInstance()->commit();
			}
			break;
		case range:
			if (result.size() >= 2) {
				FrustumCuller::getInstance()->fardist = stod(result[1]);
				FrustumCuller::getInstance()->commit();
			}
			break;
		default:
			nextLine();
			lines[lineNumber - 2] = "Invalid command";
		}
	}

public:
	string* lines;
	bool typing = false;
	int lineNumber;
	int pointer = 0;

	Console(int lineNumber) {
		this->lineNumber = lineNumber;
		lines = new string[lineNumber];
		for (int i = 0; i < lineNumber; i++) {
			lines[i] = "";
		}
		commands["speed"] = Command::speed;
		commands["go"] = Command::go;
		commands["light_ambient"] = Command::light_ambient;
		commands["light_diffuse"] = Command::light_diffuse;
		commands["light_specular"] = Command::light_specular;
		commands["light_position"] = Command::light_position;
		commands["position"] = Command::position;
		commands["range"] = Command::range;
	}

	void type(char c) {
		int mainLine = lineNumber - 1;
		switch (c) {
		case 13:
			parse();
			nextLine();
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
