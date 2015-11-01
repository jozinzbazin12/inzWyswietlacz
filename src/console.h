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
	position = 6,
	range = 7,
	LOD = 8,
	edit = 9,
	end_edit = 10,
	quit = 11
};

class Console {
private:
	int actualLine = 0;
	int mainLine;
	map<string, Command> commands;
	const string LIGHT_ERROR = "Invalid light args";

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
		case quit:
			exit(0);
		case speed:
			if (result.size() == 2) {
				predkosc = stod(result[1]);
			} else if (result.size() == 1) {

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
			} else {
				type(LIGHT_ERROR);
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
				FrustumCuller::getInstance()->update();
			}
			break;
		case LOD:
			if (result.size() >= 2) {
				lod = stod(result[1]);
			}
			break;
		case edit:
			selectedEntity = FrustumCuller::getInstance()->selected;
			if (!selectedEntity) {
				type("Nothing selected");
			} else {
				type("Selected: " + selectedEntity->object->name);
			}
			break;
		case end_edit:
			if (!selectedEntity) {
				type("Nothing selected");
			}
			selectedEntity = NULL;
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
		mainLine = lineNumber - 1;
		lines = new string[lineNumber];
		for (int i = 0; i < lineNumber; i++) {
			lines[i] = "";
		}
		commands["speed"] = Command::speed;
		commands["goto"] = Command::go;
		commands["light_ambient"] = Command::light_ambient;
		commands["light_diffuse"] = Command::light_diffuse;
		commands["light_specular"] = Command::light_specular;
		commands["light_position"] = Command::light_position;
		commands["position"] = Command::position;
		commands["range"] = Command::range;
		commands["lod"] = Command::LOD;
		commands["edit"] = Command::edit;
		commands["end"] = Command::end_edit;
		commands["quit"] = Command::quit;
		commands["exit"] = Command::quit;
	}

	void type(string str) {
		nextLine();
		lines[mainLine] = str;
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
