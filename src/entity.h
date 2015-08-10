/*
 * entity.h
 *
 *  Created on: 9 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

class Entity {
public:
	Object *object;
	Entity *parent;
	GLfloat px, py, pz;
	GLfloat sx, sy, sz;
	GLfloat rx, ry, rz;
	GLfloat min[3][3], max[3][3];
	GLfloat pomin[3][3], pomax[3][3];
	animacja *anim;
	bool alwaysDisplay;
	static vector<Entity*> allObjects;
	static vector<Entity*> sortedObjects;
	static vector<Entity*> sortedTransparentObjects;

	void recalculate() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				pomin[i][j] = min[i][j];
				pomax[i][j] = max[i][j];
			}
		updateRotation();
		updateScale();
		updatePosition();
	}

	void updateRotation() {
		const float a = 0.01745329251;
		if (ry)
			for (int i = 0; i < 3; i++) {
				pomin[i][2] = pomin[i][0] * cos(a * ry) - pomin[i][2] * sin(a * ry);
				pomin[i][0] = pomin[i][0] * sin(a * ry) + pomin[i][2] * sin(a * ry);
				pomax[i][2] = pomax[i][0] * cos(a * ry) - pomax[i][2] * sin(a * ry);
				pomax[i][0] = pomax[i][0] * sin(a * ry) + pomin[i][2] * sin(a * ry);
			}
		if (rx)
			for (int i = 0; i < 3; i++) {
				pomin[i][1] = pomin[i][1] * cos(a * rx) - pomin[i][2] * sin(a * rx);
				pomin[i][2] = pomin[i][1] * sin(a * rx) + pomin[i][2] * sin(a * rx);
				pomax[i][1] = pomax[i][1] * cos(a * rx) - pomax[i][2] * sin(a * rx);
				pomax[i][2] = pomax[i][1] * sin(a * rx) + pomin[i][2] * sin(a * rx);
			}
		if (rz)
			for (int i = 0; i < 3; i++) {
				pomin[i][0] = pomin[i][0] * cos(a * rz) - pomin[i][1] * sin(a * rz);
				pomin[i][1] = pomin[i][0] * sin(a * rz) + pomin[i][1] * sin(a * rz);
				pomax[i][0] = pomax[i][0] * cos(a * rz) - pomax[i][1] * sin(a * rz);
				pomax[i][1] = pomax[i][0] * sin(a * rz) + pomax[i][1] * sin(a * rz);
			}

	}

	void updatePosition() {
		for (int i = 0; i < 3; i++) {
			pomin[i][0] += px;
			pomax[i][0] += px;
			pomin[i][1] += py;
			pomax[i][1] += py;
			pomin[i][2] += pz;
			pomax[i][2] += pz;
		}
	}

	void updateScale() {
		for (int i = 0; i < 3; i++) {
			pomin[i][0] *= sx;
			pomax[i][0] *= sx;
			pomin[i][1] *= sy;
			pomax[i][1] *= sy;
			pomin[i][2] *= sz;
			pomax[i][2] *= sz;
		}
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

	void SetRotation(GLfloat rx, GLfloat ry, GLfloat rz) {
		this->rx = rx;
		this->ry = ry;
		this->rz = rz;
	}

	Entity(Object *object) {
		alwaysDisplay = false;
		anim = NULL;
		parent = NULL;
		this->object = object;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				this->min[i][j] = object->min[i][j];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				this->max[i][j] = object->max[i][j];
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
		Logger::log("Tworze obiekt " + this->object->name, true);
	}

	~Entity() {
		Logger::log("Usuwam obiekt " + this->object->name, true);
		for (int i = 0; i < allObjects.size(); i++) {
			if (allObjects[i]->parent == this) {
				allObjects[i]->parent = parent;
			}
		}
		for (unsigned i = 0; i < sortedObjects.size(); i++) {
			if (sortedObjects[i] == this) {
				sortedObjects[i] = NULL;
			}
		}
		for (unsigned i = 0; i < sortedTransparentObjects.size(); i++) {
			if (sortedTransparentObjects[i] == this) {
				sortedTransparentObjects[i] = NULL;
			}
		}
		object->counter--;
	}
};

vector<Entity*> Entity::allObjects;
vector<Entity*> Entity::sortedObjects;
vector<Entity*> Entity::sortedTransparentObjects;

#endif /* SRC_ENTITY_H_ */
