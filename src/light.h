/*
 * light.h
 *
 *  Created on: 28 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_LIGHT_H_
#define SRC_LIGHT_H_

class Light {
private:
	static Light* instance;
public:
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 0.1f };
	GLfloat diffuse[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	GLfloat specular[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
	GLfloat position[4] = { 2.0f, 5.0f, 5.0f, 0.0f };

	static Light* getInstance() {
		if (!instance) {
			instance = new Light();
		}
		return instance;
	}

	void setAmbient(double r, double g, double b, double a) {
		ambient[0] = r;
		ambient[1] = g;
		ambient[2] = b;
		ambient[3] = a;
	}

	void setDiffuse(double r, double g, double b, double a) {
		diffuse[0] = r;
		diffuse[1] = g;
		diffuse[2] = b;
		diffuse[3] = a;
	}

	void setSpecular(double r, double g, double b, double a) {
		specular[0] = r;
		specular[1] = g;
		specular[2] = b;
		specular[3] = a;
	}

	void setPosition(double x, double y, double z, double mode) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
		position[3] = mode;
	}

	void commit() {
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
	}
};
Light* Light::instance=NULL;

#endif /* SRC_LIGHT_H_ */
