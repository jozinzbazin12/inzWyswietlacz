/*
 * Material.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MATERIAL_H_
#define SRC_MATERIAL_H_

class Material {
public:
	GLenum s;
	float kat[4];
	float kdt[4];
	float kst[4];
	int tkat;
	int tkdt;
	int tkst;
	float nst;
	string imie;

	void setD(float d) {
		this->kat[3] = d;
		this->kdt[3] = d;
		this->kst[3] = d;
	}
	void setNst(float nst) {
		this->nst = nst;
	}

	void setAmbient(float kat[3]) {
		this->kat[0] = kat[0];
		this->kat[1] = kat[1];
		this->kat[2] = kat[2];
	}

	void setDiffuse(float kdt[3]) {
		this->kdt[0] = kdt[0];
		this->kdt[1] = kdt[1];
		this->kdt[2] = kdt[2];
	}

	void setSpecular(float kst[3]) {
		this->kst[0] = kst[0];
		this->kst[1] = kst[1];
		this->kst[2] = kst[2];
	}

	void setMapKd(int tkdt) {
		this->tkdt = tkdt;
	}

	Material(string imie) {
		tkat = -1;
		tkdt = -1;
		tkst = -1;
		s = GL_SMOOTH;
		this->imie = imie;
	}
};

#endif /* SRC_MATERIAL_H_ */
