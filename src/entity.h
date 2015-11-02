/*
 * entity.h
 *
 *  Created on: 9 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

class Entity: public Cullable {
private:
	static HANDLE mutex;

	void updateScale() {
		furthest[0] *= sx;
		furthest[1] *= sy;
		furthest[2] *= sz;
	}

	void copyFurthest() {
		furthest[0] = object->furthest[0];
		furthest[1] = object->furthest[1];
		furthest[2] = object->furthest[2];
	}

	void deleteFromTree();
public:
	static TreeNode* objects;
	static unsigned long entitiesCount;
	double furthest[3];
	Object *object;
	GLfloat px, py, pz;
	GLfloat sx, sy, sz;
	GLfloat rx, ry, rz;
	Animation *anim;
	bool alwaysDisplay;
	static list<Entity*> solidObjectsToDisplay;
	static list<Entity*> transparentObjectsToDisplay;

	double* getPos() {
		double* tab = new double[3];
		tab[0] = px;
		tab[1] = py;
		tab[2] = pz;
		return tab;
	}

	static void addEntity(Entity* entity);

	static bool compare(Entity* e1, Entity* e2) {
		int val = pow(posX - e1->px, 2) + pow(posY - e1->py, 2) + pow(posZ - e1->pz, 2);
		int val2 = pow(posX - e2->px, 2) + pow(posY - e2->py, 2) + pow(posZ - e2->pz, 2);
		return val > val2;
	}

	void recalculate() {
		copyFurthest();
		updateScale();
		range = getLength3D(&furthest[0], &empty[0]);
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
		recalculate();
	}

	void setRotation(GLfloat rx, GLfloat ry, GLfloat rz) {
		this->rx = rx;
		this->ry = ry;
		this->rz = rz;
	}

	Entity(Object *object) {
		Logger::log("Tworzê obiekt " + object->name);
		alwaysDisplay = false;
		entitiesCount++;
		anim = NULL;
		this->object = object;
		copyFurthest();
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
		recalculate();
	}

	~Entity() {
		object->counter--;
		entitiesCount--;
		deleteFromTree();
	}
};
HANDLE Entity::mutex = CreateMutex(NULL, FALSE, NULL);
unsigned long Entity::entitiesCount = 0;
list<Entity*> Entity::solidObjectsToDisplay;
list<Entity*> Entity::transparentObjectsToDisplay;
TreeNode* Entity::objects = NULL;

#endif /* SRC_ENTITY_H_ */
