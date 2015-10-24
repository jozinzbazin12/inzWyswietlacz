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

	void caclulate(float tab[3], float tab2[3], float &p1, float &p2, float md1, float md2, float md3) {
		float v[3], v2[3];
		v[0] = tab[0] - px2;
		v[1] = tab[1] - py2;
		v[2] = tab[2] - pz2;
		p1 = -md1 * v[0] - md2 * v[1] - md3 * v[2];
		v2[0] = tab2[0] - px2;
		v2[1] = tab2[1] - py2;
		v2[2] = tab2[2] - pz2;
		p2 = -md1 * v2[0] - md2 * v2[1] - md3 * v2[2];
	}

	bool checkX(float tab[3], float tab2[3]) {
		h = pcz * tan;
		h2 = pcz2 * tan;
		float w = h * 2.0 * ar / (top - bottom);
		float w2 = h2 * 2.0 * ar / (top - bottom);
		caclulate(tab, tab2, pcx, pcx2, modelview[0], modelview[4], modelview[8]);
		if ((pcx > -w && pcx < w) || (pcx2 > -w2 && pcx2 < w2)) {
			return true;
		}
		return false;
	}

	bool chechY(float tab[3], float tab2[3]) {
		h = pcz * tan;
		h2 = pcz2 * tan;
		caclulate(tab, tab2, pcy, pcy2, modelview[1], modelview[5], modelview[9]);
		if ((pcy > -h && pcy < h) || (pcy2 > -h2 && pcy2 < h2)) {
			return true;
		}
		return false;
	}

	bool checkZ(float tab[3], float tab2[3]) {
		caclulate(tab, tab2, pcz, pcz2, modelview[2], modelview[6], modelview[10]);
		if ((pcz < fardist && pcz > neardist) || (pcz2 < fardist && pcz2 > neardist)) {
			return true;
		}
		return false;
	}

	FrustumCuller() {
	}

public:
	float fardist = 100000.0;
	float neardist = 1.0;
	float bottom = -0.5;
	float top = 0.5;
	float tan = (top - bottom) / neardist;
	float ar;
	float pcx, pcx2, pcy, pcy2, pcz, pcz2;
	float h, h2;
	GLfloat px2, py2, pz2;

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
		if (selectedEntityPos == -1) {
			px2 = posX;
			py2 = posY;
			pz2 = posZ;
		} else {
			pz2 = cameraDistance;
			py2 = -pz2 * sin(-RAD * cx);
			pz2 = pz2 * cos(-RAD * cx);
			px2 = -pz2 * sin(RAD * cy);
			pz2 = pz2 * cos(RAD * cy);

			px2 += selectedEntity->px;
			py2 += selectedEntity->py + 5;
			pz2 += selectedEntity->pz;
		}

		e->recalculate();
		if (checkZ(e->realMin[2], e->realMax[2]) && chechY(e->realMin[1], e->realMax[1]) && checkX(e->realMin[0], e->realMax[0])) {
			return true;
		}
		return false;
	}

	bool isInViewField(TreeNode* node) {
		px2 = posX;
		py2 = posY;
		pz2 = posZ;
//		if (checkZ(node->se[1], node->nw[1]) && checkX(node->ne[0], node->sw[0])) {
//			return true;
//		}
		return true;
	}

	void commit() {
		glFrustum(-ar, ar, bottom, top, neardist, fardist);
	}
};
FrustumCuller* FrustumCuller::culler = NULL;

#endif /* SRC_FRUSTUM_CULLER_H_ */
