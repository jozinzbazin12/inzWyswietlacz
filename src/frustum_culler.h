/*
 * frustum_culler.h
 *
 *  Created on: 10 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_FRUSTUM_CULLER_H_
#define SRC_FRUSTUM_CULLER_H_

class FrustumCuller {
private:
	static FrustumCuller* culler;
	static constexpr double RAD = 0.01745329251;

	double caclulate(double tab[3], float md1, float md2, float md3) {
		double v[3];
		v[0] = px2 - tab[0];
		v[1] = py2 - tab[1];
		v[2] = pz2 - tab[2];
		delete[] tab;
		return md1 * v[0] + md2 * v[1] + md3 * v[2];
	}

	bool checkX(Cullable* e) {
		double* tab = e->getPos();
		h = pcz * tan;
		double w = h * ar / (top - bottom);
		pcx = caclulate(tab, modelview[0], modelview[4], modelview[8]);
		return (pcx >= -w && pcx <= w) || (pcx + e->range >= -w && pcx - e->range <= w);
	}

	bool checkY(Cullable* e) {
		double* tab = e->getPos();
		h = pcz * tan;
		double h2 = h / 2;
		pcy = caclulate(tab, modelview[1], modelview[5], modelview[9]);
		return (pcy >= -h2 && pcy <= h2) || (pcy + e->range >= -h2 && pcy - e->range <= h2);
	}

	bool checkZ(Cullable* e) {
		double* tab = e->getPos();
		pcz = caclulate(tab, modelview[2], modelview[6], modelview[10]);
		return (pcz <= fardist && pcz >= neardist) || (pcz - e->range <= fardist && pcz - e->range >= neardist);
	}

	void select(Entity* e) {
		pcx = caclulate(e->getPos(), modelview[0], modelview[4], modelview[8]);
		pcy = caclulate(e->getPos(), modelview[1], modelview[5], modelview[9]);
		pcz = caclulate(e->getPos(), modelview[2], modelview[6], modelview[10]);
		double pcz2 = selected ? caclulate(selected->getPos(), modelview[2], modelview[6], modelview[10]) : INFINITE;
		if (pcz < pcz2 && pcz > 0 && abs(pcy) <= e->range && abs(pcx) <= e->range) {
			selected = e;
		}
	}

	bool inSpehere(Cullable* e) {
		double* tab = e->getPos();
		double dist = distance(posX, tab[0], posY, tab[1], posZ, tab[2]);
		return dist <= e->range;
	}

	double distance(double x1, double x2, double y1, double y2, double z1, double z2) {
		return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
	}

	FrustumCuller() {
	}

public:
	float fardist = 100000.0;
	float neardist = 1.0;
	float bottom = -0.5;
	float top = 0.5;
	float tan = 1;
	float ar;
	float pcx, pcy, pcz;
	float h;
	GLfloat px2, py2, pz2;
	Entity* selected = NULL;

	static FrustumCuller* getInstance() {
		if (!culler) {
			culler = new FrustumCuller();
		}
		return culler;
	}

	bool isInViewField(Entity* e) {
		if (e->alwaysDisplay) {
			return true;
		}
		if (e->range * (e->sx + e->sy + e->sz) / 3.0 < pow(distance(posX, e->px, posY, e->py, posZ, e->pz) / lod, 2)) {
			return false;
		}
		if (!selectedEntity) {
			px2 = posX;
			py2 = posY;
			pz2 = posZ;
		} else {
			pz2 = cameraDistance;
			py2 = -pz2 * sin(RAD * cx);
			pz2 = pz2 * cos(RAD * cx);
			px2 = -pz2 * sin(-RAD * cy);
			pz2 = pz2 * cos(-RAD * cy);

			px2 += selectedEntity->px;
			py2 += selectedEntity->py + cameraDistance;
			pz2 += selectedEntity->pz;
		}

		if (inSpehere(e) || (checkZ(e) && checkY(e) && checkX(e))) {
			select(e);
			return true;
		}
		return false;
	}

	bool isInViewField(TreeNode* node) {
		px2 = posX;
		py2 = posY;
		pz2 = posZ;
		return inSpehere(node) || (checkZ(node) && checkX(node));
	}

	void commit(int width, int height) {
		ar = (float) width / (float) height / 2;
		glViewport(0, 0, width, height);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity();
		update();
		glMatrixMode (GL_MODELVIEW);
		windowHeight = height;
		windowWidth = width;
	}

	void update() {
		glFrustum(-ar, ar, bottom, top, neardist, fardist);
		tan = (top - bottom) / neardist;
	}
}
;
FrustumCuller* FrustumCuller::culler = NULL;

#endif /* SRC_FRUSTUM_CULLER_H_ */
