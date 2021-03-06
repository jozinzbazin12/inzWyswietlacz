/*
 * tree.h
 *
 *  Created on: 21 pa� 2015
 *      Author: Grzegurz
 */

#ifndef SRC_TREE_H_
#define SRC_TREE_H_

class TreeNode: public Cullable {
private:
	static const short NE = 0;
	static const short NW = 1;
	static const short SE = 2;
	static const short SW = 3;
	static HANDLE entitiesMutex;
	static HANDLE nodesMutex;
	static TreeNode* root;
	double* mid;
	vector<Entity*> entities;

	static void setPoint(double* tab, int x, int z) {
		tab[0] = x;
		tab[1] = z;
	}

	void init() {
		ne = new double[2];
		nw = new double[2];
		se = new double[2];
		sw = new double[2];
		mid = new double[2];
		children = new TreeNode*[4];
		for (int i = 0; i < 4; i++) {
			children[i] = NULL;
		}
	}

	TreeNode* getNode(int index) {
		WaitForSingleObject(nodesMutex, INFINITE);
		if (!children[index]) {
			children[index] = new TreeNode(this, index);
		}
		ReleaseMutex(nodesMutex);
		return children[index];
	}

	bool shouldBeInNextNode(TreeNode* node, Entity* e) {
		return node->level < node->LEVELS && e->range < node->range
				&& getLength2D(e->px, node->mid[0], e->pz, node->mid[1]) + e->range < node->range;
	}

public:
	TreeNode** children;
	static const short LEVELS = 3;
	int level;
	double* ne;
	double* nw;
	double* se;
	double* sw;

	unsigned getSize() {
		return entities.size();
	}

	Entity* getEntity(unsigned pos) {
		Entity* e = NULL;
		WaitForSingleObject(entitiesMutex, INFINITE);
		e = entities[pos];
		ReleaseMutex(entitiesMutex);
		return e;
	}

	double* getPos() {
		double* tab = new double[3];
		tab[0] = mid[0];
		tab[1] = 0;
		tab[2] = mid[1];
		return tab;
	}

	static TreeNode* createRoot(double width, double height) {
		TreeNode* n = new TreeNode();
		int w = width / 2;
		int h = height / 2;
		setPoint(n->mid, 0, 0);
		setPoint(n->ne, w, h);
		setPoint(n->nw, -w, h);
		setPoint(n->se, w, -h);
		setPoint(n->sw, -w, -h);
		n->range = getLength2D(n->ne, n->mid);
		n->level = 0;
		return n;
	}

	TreeNode* getChild(Entity* e) {
		double midX = mid[0];
		double midZ = mid[1];
		double x = e->px;
		double z = e->pz;
		if (x >= midX && z >= midZ) {
			return getNode(NE);
		}
		if (x <= midX && z > midZ) {
			return getNode(NW);
		}
		if (x > midX && z <= midZ) {
			return getNode(SE);
		}
		if (x <= midX && z < midZ) {
			return getNode(SW);
		}
		return NULL;
	}

	void deleteOb(Entity* e) {
		TreeNode* node = this;
		while (shouldBeInNextNode(node, e)) {
			node = node->getChild(e);
		}

		for (unsigned i = 0; i < entities.size(); i++) {
			if (entities[i] == e) {
				WaitForSingleObject(entitiesMutex, INFINITE);
				entities.erase(entities.begin() + i);
				ReleaseMutex(entitiesMutex);
				break;
			}
		}
	}

	void addObject(Entity* e) {
		TreeNode* node = this;
		while (shouldBeInNextNode(node, e)) {
			node = node->getChild(e);
		}
		WaitForSingleObject(entitiesMutex, INFINITE);
		node->entities.push_back(e);
		ReleaseMutex(entitiesMutex);
	}

	TreeNode(TreeNode* node, short part) {
		init();
		double w = getLength2D(node->ne, node->nw) / 4.0;
		double h = getLength2D(node->ne, node->se) / 4.0;
		range = node->range / 2.0;

		if (part == NE) {
			setPoint(mid, node->mid[0] + w, node->mid[1] + h);
		}
		if (part == NW) {
			setPoint(mid, node->mid[0] - w, node->mid[1] + h);
		}
		if (part == SE) {
			setPoint(mid, node->mid[0] + w, node->mid[1] - h);
		}
		if (part == SW) {
			setPoint(mid, node->mid[0] - w, node->mid[1] - h);
		}

		setPoint(ne, mid[0] + w, mid[1] + h);
		setPoint(nw, mid[0] - w, mid[1] + h);
		setPoint(se, mid[0] + w, mid[1] - h);
		setPoint(sw, mid[0] - w, mid[1] - h);

		level = node->level + 1;
	}

	TreeNode() {
		init();
	}
};
HANDLE TreeNode::entitiesMutex = CreateMutexA(NULL, false, NULL);
HANDLE TreeNode::nodesMutex = CreateMutexA(NULL, false, NULL);

#endif /* SRC_TREE_H_ */
