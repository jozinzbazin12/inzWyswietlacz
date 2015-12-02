/*
 * console_actions.h
 *
 *  Created on: 1 lis 2015
 *      Author: Grzegurz
 */

#ifndef SRC_CONSOLE_ACTIONS_H_
#define SRC_CONSOLE_ACTIONS_H_

class Action {
protected:
	unsigned short minArg = 0;
	unsigned short maxArg = 0;
	static string invalidArgs;

	virtual void noArgAction(Console* c, vector<string> args)=0;
	virtual void okArgsAction(Console* c, vector<string> args)=0;
	void invalidArgsAction(Console* c, vector<string> args) {
		c->type(invalidArgs);
	}
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}

	string arg(int a) {
		ostringstream os;
		os << a;
		return os.str();
	}
public:
	void execute(Console* c, vector<string> args) {
		int size = args.size() - 1;
		if (size == 0) {
			noArgAction(c, args);
		} else if ((size == minArg || size == maxArg) && validateArgs(c, args)) {
			okArgsAction(c, args);
		} else {
			invalidArgsAction(c, args);
		}
	}

	virtual ~Action() {

	}
};
string Action::invalidArgs = "Ivalid args";

class HelpAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		string str = "Available commands: ";
		int rowCount = 0;
		for (map<string, Action*>::iterator it = c->commands.begin(); it != c->commands.end(); ++it) {
			str += it->first + ", ";
			if (rowCount++ == 5) {
				str += "\n";
				rowCount = 0;
			}
		}
		c->type(str);
	}
public:
	HelpAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class ClearAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		c->clear();
	}
public:
	ClearAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class SpeedAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return stod(args[1]) > 0;
	}
	void noArgAction(Console* c, vector<string> args) {
		c->type(arg(cameraSpeed));
	}
	void okArgsAction(Console* c, vector<string> args) {
		cameraSpeed = stod(args[1]);
	}
public:
	SpeedAction() {
		minArg = 0;
		maxArg = 1;
	}
};

class ExitAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		_Exit(0);
	}
public:
	ExitAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class GotoAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		invalidArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		posX = stod(args[1]);
		posY = stod(args[2]);
		posZ = stod(args[3]);
	}
public:
	GotoAction() {
		minArg = 3;
		maxArg = 3;
	}
};

class LightAction: public Action {
protected:
	string arg(GLfloat* l) {
		ostringstream os;
		os << "R: " << l[0] << " G: " << l[1] << " B: " << l[2] << " A: " << l[3];
		return os.str();
	}
	bool isCorrect(string arg) {
		int value = stod(arg);
		return value >= 0 && value <= 1;
	}
	bool validateArgs(Console* c, vector<string> args) {
		return isCorrect(args[1]) && isCorrect(args[2]) && isCorrect(args[3]) && isCorrect(args[4]);
	}
	void okArgsAction(Console* c, vector<string> args) {
		posX = stod(args[1]);
		posY = stod(args[2]);
		posZ = stod(args[3]);
	}
public:
	LightAction() {
		minArg = 4;
		maxArg = 4;
	}
};

class LightAmbientAction: public LightAction {
private:
	void noArgAction(Console* c, vector<string> args) {
		GLfloat* l = Light::getInstance()->ambient;
		c->type(arg(l));
	}
	void okArgsAction(Console* c, vector<string> args) {
		Light::getInstance()->setAmbient(stod(args[1]), stod(args[2]), stod(args[3]), stod(args[4]));
		Light::getInstance()->commit();
	}
};

class LightDiffuseAction: public LightAction {
private:
	void noArgAction(Console* c, vector<string> args) {
		GLfloat* l = Light::getInstance()->diffuse;
		c->type(arg(l));
	}
	void okArgsAction(Console* c, vector<string> args) {
		Light::getInstance()->setDiffuse(stod(args[1]), stod(args[2]), stod(args[3]), stod(args[4]));
		Light::getInstance()->commit();
	}
};

class LightSpecularAction: public LightAction {
private:
	void noArgAction(Console* c, vector<string> args) {
		GLfloat* l = Light::getInstance()->diffuse;
		c->type(arg(l));
	}
	void okArgsAction(Console* c, vector<string> args) {
		Light::getInstance()->setSpecular(stod(args[1]), stod(args[2]), stod(args[3]), stod(args[4]));
		Light::getInstance()->commit();
	}
};

class LightPositionAction: public LightAction {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return isCorrect(args[1]) && isCorrect(args[2]) && isCorrect(args[3]);
	}

	void noArgAction(Console* c, vector<string> args) {
		GLfloat* l = Light::getInstance()->position;
		c->type(arg(l));
	}
	void okArgsAction(Console* c, vector<string> args) {
		Light::getInstance()->setPosition(stod(args[1]), stod(args[2]), stod(args[3]), stod(args[4]));
		Light::getInstance()->commit();
	}
};

class RangeAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return stod(args[1]) > 0;
	}
	void noArgAction(Console* c, vector<string> args) {
		c->type(arg(FrustumCuller::getInstance()->fardist));
	}
	void okArgsAction(Console* c, vector<string> args) {
		FrustumCuller::getInstance()->fardist = stod(args[1]);
		FrustumCuller::getInstance()->update();
	}
public:
	RangeAction() {
		minArg = 0;
		maxArg = 1;
	}
};

class LODAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return stod(args[1]) > 0;
	}
	void noArgAction(Console* c, vector<string> args) {
		c->type(arg(lod));
	}
	void okArgsAction(Console* c, vector<string> args) {
		lod = stod(args[1]);
	}
public:
	LODAction() {
		minArg = 0;
		maxArg = 1;
	}
};

class EditEntityAction: public Action {
private:
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		selectedEntity = FrustumCuller::getInstance()->selected;
		if (!selectedEntity) {
			c->type("Nothing selected");
		} else {
			posX = selectedEntity->px;
			posY = selectedEntity->py;
			posZ = selectedEntity->pz;
			c->type("Selected: " + selectedEntity->object->name);
			Entity::objects->deleteOb(selectedEntity);
			Entity::movingObjects.push_back(selectedEntity);
		}
	}
public:
	EditEntityAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class EndEditEntityAction: public Action {
private:
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		if (!selectedEntity) {
			c->type("Nothing selected");
			return;
		}
		Entity::movingObjects.remove(selectedEntity);
		Entity::addEntity(selectedEntity);
		selectedEntity = NULL;
	}
public:
	EndEditEntityAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class ScaleEntityAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return selectedEntity != NULL;
	}
	void noArgAction(Console* c, vector<string> args) {
		ostringstream os;
		os << "X: " << selectedEntity->sx << " Y: " << selectedEntity->sy << " Z: " << selectedEntity->sz;
		c->type(os.str());
	}
	void okArgsAction(Console* c, vector<string> args) {
		selectedEntity->sx = stod(args[1]);
		selectedEntity->sy = stod(args[2]);
		selectedEntity->sz = stod(args[3]);
	}
public:
	ScaleEntityAction() {
		minArg = 0;
		maxArg = 3;
	}
};

class RotateEntityAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return selectedEntity != NULL;
	}
	void noArgAction(Console* c, vector<string> args) {
		ostringstream os;
		os << "X: " << selectedEntity->rx << " Y: " << selectedEntity->ry << " Z: " << selectedEntity->rz;
		c->type(os.str());
	}
	void okArgsAction(Console* c, vector<string> args) {
		selectedEntity->rx = stod(args[1]);
		selectedEntity->ry = stod(args[2]);
		selectedEntity->rz = stod(args[3]);
	}
public:
	RotateEntityAction() {
		minArg = 0;
		maxArg = 3;
	}
};

class DeleteEntityAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return selectedEntity != NULL;
	}
	void noArgAction(Console* c, vector<string> args) {
		okArgsAction(c, args);
	}
	void okArgsAction(Console* c, vector<string> args) {
		delete selectedEntity;
		selectedEntity = NULL;
		FrustumCuller::getInstance()->selected = NULL;
		c->type("Entity deleted");
	}

public:
	DeleteEntityAction() {
		minArg = 0;
		maxArg = 0;
	}
};

class NewEntityAction: public Action {
private:
	void noArgAction(Console* c, vector<string> args) {
		Entity* e = new Entity(Object::getObject(selectedObjectPos));
		selectedEntity = e;
		Entity::movingObjects.push_back(e);
		c->type("Entity created");
	}

	void okArgsAction(Console* c, vector<string> args) {
		noArgAction(c, args);
		posX = stod(args[1]);
		posY = stod(args[2]);
		posZ = stod(args[3]);
	}

public:
	NewEntityAction() {
		minArg = 0;
		maxArg = 3;
	}
};

class SkyAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		c->type(skyEnabled ? "true" : "false");
	}
	void okArgsAction(Console* c, vector<string> args) {
		skyEnabled = args[1] == "1" || args[1] == "true";
	}
public:
	SkyAction() {
		minArg = 0;
		maxArg = 1;
	}
};

class CageAction: public Action {
private:
	bool validateArgs(Console* c, vector<string> args) {
		return true;
	}
	void noArgAction(Console* c, vector<string> args) {
		c->type(cagesEnabled ? "true" : "false");
	}
	void okArgsAction(Console* c, vector<string> args) {
		cagesEnabled = args[1] == "1" || args[1] == "true";
	}
public:
	CageAction() {
		minArg = 0;
		maxArg = 1;
	}
};


#endif /* SRC_CONSOLE_ACTIONS_H_ */
