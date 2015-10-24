/*
 * entity.h
 *
 *  Created on: 9 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

class Entity {
private:
	static vector<Entity*> allObjects;
	static HANDLE mutex;

	void updateRotation() {
		const float a = 0.01745329251;
		if (ry)
			for (int i = 0; i < 3; i++) {
				realMin[i][2] = realMin[i][0] * cos(a * ry) - realMin[i][2] * sin(a * ry);
				realMin[i][0] = realMin[i][0] * sin(a * ry) + realMin[i][2] * sin(a * ry);
				realMax[i][2] = realMax[i][0] * cos(a * ry) - realMax[i][2] * sin(a * ry);
				realMax[i][0] = realMax[i][0] * sin(a * ry) + realMin[i][2] * sin(a * ry);
			}
		if (rx)
			for (int i = 0; i < 3; i++) {
				realMin[i][1] = realMin[i][1] * cos(a * rx) - realMin[i][2] * sin(a * rx);
				realMin[i][2] = realMin[i][1] * sin(a * rx) + realMin[i][2] * sin(a * rx);
				realMax[i][1] = realMax[i][1] * cos(a * rx) - realMax[i][2] * sin(a * rx);
				realMax[i][2] = realMax[i][1] * sin(a * rx) + realMin[i][2] * sin(a * rx);
			}
		if (rz)
			for (int i = 0; i < 3; i++) {
				realMin[i][0] = realMin[i][0] * cos(a * rz) - realMin[i][1] * sin(a * rz);
				realMin[i][1] = realMin[i][0] * sin(a * rz) + realMin[i][1] * sin(a * rz);
				realMax[i][0] = realMax[i][0] * cos(a * rz) - realMax[i][1] * sin(a * rz);
				realMax[i][1] = realMax[i][0] * sin(a * rz) + realMax[i][1] * sin(a * rz);
			}
	}

	void updatePosition() {
		for (int i = 0; i < 3; i++) {
			realMin[i][0] += px;
			realMax[i][0] += px;
			realMin[i][1] += py;
			realMax[i][1] += py;
			realMin[i][2] += pz;
			realMax[i][2] += pz;
		}
	}

	void updateScale() {
		for (int i = 0; i < 3; i++) {
			realMin[i][0] *= sx;
			realMax[i][0] *= sx;
			realMin[i][1] *= sy;
			realMax[i][1] *= sy;
			realMin[i][2] *= sz;
			realMax[i][2] *= sz;
		}
	}
public:
	static TreeNode* objects;
	Object *object;
	GLfloat px, py, pz;
	GLfloat sx, sy, sz;
	GLfloat rx, ry, rz;
	GLfloat min[3][3], max[3][3];
	GLfloat realMin[3][3], realMax[3][3];
	Animation *anim;
	bool alwaysDisplay;
	static vector<Entity*> solidObjectsToDisplay;
	static vector<Entity*> transparentObjectsToDisplay;

	static Entity* getEntity(int pos) {
		WaitForSingleObject(mutex, INFINITE);
		Entity* result = allObjects[pos];
		ReleaseMutex(mutex);
		return result;
	}

	static void addEntity(Entity* entity);

	static int allEntitiesCount() {
		return allObjects.size();
	}

	static void setEntity(Entity* e, int pos) {
		allObjects[pos] = e;
	}
	static bool compare(Entity* e1, Entity* e2) {
		int val = pow(posX - e1->px, 2) + pow(posY - e1->py, 2) + pow(posZ - e1->pz, 2);
		int val2 = pow(posX - e2->px, 2) + pow(posY - e2->py, 2) + pow(posZ - e2->pz, 2);
		return val > val2;
	}

	void recalculate() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				realMin[i][j] = min[i][j];
				realMax[i][j] = max[i][j];
			}
		updateRotation();
		updateScale();
		updatePosition();
	}

	void setPosition(GLfloat px, GLfloat py, GLfloat pz) {
		this->px = px;
		this->py = py;
		this->pz = pz;
	}

	void setScale(GLfloat sx, GLfloat sy, GLfloat sz) {
		this->sx = sx;
		this->sy = sy;
		this->sz = sz;
	}

	void setRotation(GLfloat rx, GLfloat ry, GLfloat rz) {
		this->rx = rx;
		this->ry = ry;
		this->rz = rz;
	}

	Entity(Object *object) {
		Logger::log("Tworzê obiekt " + object->name);
		alwaysDisplay = false;
		anim = NULL;
		this->object = object;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->min[i][j] = object->min[i][j];
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->max[i][j] = object->max[i][j];
			}
		}
		this->px = 0;
		this->py = 0;
		this->pz = 0;
		this->sx = 1;
		this->sy = 1;
		this->sz = 1;
		this->rx = 0;
		this->ry = 0;
		this->rz = 0;
		object->counter++;
	}

	~Entity() {
		Logger::log("Usuwam obiekt " + this->object->name, true);
		for (unsigned i = 0; i < solidObjectsToDisplay.size(); i++) {
			if (solidObjectsToDisplay[i] == this) {
				solidObjectsToDisplay[i] = NULL;
			}
		}
		for (unsigned i = 0; i < transparentObjectsToDisplay.size(); i++) {
			if (transparentObjectsToDisplay[i] == this) {
				transparentObjectsToDisplay[i] = NULL;
			}
		}
		object->counter--;
	}
};
HANDLE Entity::mutex = CreateMutex(NULL, FALSE, NULL);
vector<Entity*> Entity::allObjects;
vector<Entity*> Entity::solidObjectsToDisplay;
vector<Entity*> Entity::transparentObjectsToDisplay;
TreeNode* Entity::objects = NULL;

#endif /* SRC_ENTITY_H_ */
