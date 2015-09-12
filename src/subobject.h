/*
 * suboject.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_SUBOBJECT_H_
#define SRC_SUBOBJECT_H_

class Subobject {
public:
	long long unsigned vertexCount;
	Material *mtl;
	GLuint vertices;
	GLuint texture;
	GLuint normals;

	Subobject(int vertexCount, Material* mtlNumber, GLuint* buffers) {
		this->vertexCount = vertexCount;
		this->mtl = mtlNumber;
		vertices = buffers[0];
		normals = buffers[1];
		texture = buffers[2];
	}
};

#endif /* SRC_SUBOBJECT_H_ */
