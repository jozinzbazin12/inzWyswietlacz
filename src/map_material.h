/*
 * map_material.h
 *
 *  Created on: 13 wrz 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MAP_MATERIAL_H_
#define SRC_MAP_MATERIAL_H_

class MapMaterial {
public:
	double* ka;
	double* kd;
	double* ks;
	double ns;
	double d;

	~MapMaterial() {
		delete[] ka;
		delete[] kd;
		delete[] ks;
	}

	void setKa(double r, double g, double b) {
		double* tab = new double[3];
		tab[0] = r;
		tab[1] = g;
		tab[2] = b;
		ka = tab;
	}

	void setKd(double r, double g, double b) {
		double* tab = new double[3];
		tab[0] = r;
		tab[1] = g;
		tab[2] = b;
		kd = tab;
	}

	void setKs(double r, double g, double b) {
		double* tab = new double[3];
		tab[0] = r;
		tab[1] = g;
		tab[2] = b;
		ks = tab;
	}
};

#endif /* SRC_MAP_MATERIAL_H_ */
