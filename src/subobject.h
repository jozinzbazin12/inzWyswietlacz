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
	int ktorybuff[3];

	Subobject(int vertexCount, int mtlNumber, MaterialLib* mtl, int bufferNumber) {
		this->vertexCount = vertexCount;
		this->mtl = mtl->mtl[mtlNumber];
		this->ktorybuff[0] = bufferNumber;
		this->ktorybuff[1] = bufferNumber + 1;
		this->ktorybuff[2] = bufferNumber + 2;
	}
};

#endif /* SRC_SUBOBJECT_H_ */
