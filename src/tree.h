/*
 * tree.h
 *
 *  Created on: 21 paü 2015
 *      Author: Grzegurz
 */

#ifndef SRC_TREE_H_
#define SRC_TREE_H_

class TreeNode {
private:
	static const short NE = 0;
	static const short NW = 1;
	static const short SE = 2;
	static const short SW = 3;

	static TreeNode* root;
	double* mid;

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

	TreeNode* getNode(int index);

public:
	TreeNode** children;
	static const short LEVELS = 3;
	int level;
	double* ne;
	double* nw;
	double* se;
	double* sw;

	static TreeNode* createRoot(double width, double height) {
		TreeNode* n = new TreeNode();
		int w = width / 2;
		int h = height / 2;
		setPoint(n->mid, 0, 0);
		setPoint(n->ne, -w, h);
		setPoint(n->nw, w, h);
		setPoint(n->se, -w, -h);
		setPoint(n->sw, w, -h);
		n->level = 0;
		return n;
	}

	double getLength(double* p1, double* p2) {
		return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2));
	}

	TreeNode* getChild(Entity* e) {
		double midX = getLength(ne, se);
		double midZ = getLength(ne, nw);
		double x = e->px;
		double z = e->pz;
		if (x > midX && z > midZ) {
			return getNode(NE);
		}
		if (x < midX && z > midZ) {
			return getNode(NW);
		}
		if (x < midX && z < midZ) {
			return getNode(SE);
		}
		if (x > midX && z < midZ) {
			return getNode(SW);
		}
		return NULL;
	}

	void addObject(Entity* e);

	TreeNode(TreeNode* node, short part) {
		init();
		double w = getLength(node->ne, node->nw) / 2;
		double h = getLength(node->ne, node->se) / 2;

		if (part == NE) {
			setPoint(ne, node->ne[0], node->ne[1]);
			setPoint(nw, node->nw[0] - w, node->nw[1]);
			setPoint(se, node->se[0], node->se[1] - h);
			setPoint(sw, node->sw[0] - w, node->sw[1] - h);
		}
		if (part == NW) {
			setPoint(ne, node->ne[0] + w, node->ne[1]);
			setPoint(nw, node->nw[0], node->nw[1]);
			setPoint(se, node->se[0] + w, node->se[1] - h);
			setPoint(sw, node->sw[0], node->sw[1] - h);
		}
		if (part == SE) {
			setPoint(ne, node->ne[0], node->ne[1] - h);
			setPoint(nw, node->nw[0] - w, node->nw[1] - h);
			setPoint(se, node->se[0], node->se[1]);
			setPoint(sw, node->sw[0] - w, node->sw[1]);
		}
		if (part == SW) {
			setPoint(ne, node->ne[0] + w, node->ne[1] - h);
			setPoint(nw, node->nw[0], node->nw[1] - h);
			setPoint(se, node->se[0] + w, node->se[1]);
			setPoint(sw, node->sw[0], node->sw[1]);
		}

		level = node->level + 1;
	}

	TreeNode() {
		init();
	}
};

class TreeLeaf: public TreeNode {
public:
	vector<Entity*> entities;
	void addObject(Entity* e) {
		entities.push_back(e);
	}

	TreeLeaf() {
		level = LEVELS + 1;
	}
};

TreeNode* TreeNode::getNode(int index) {
	if (!children[index]) {
		if (level == LEVELS) {
			children[index] = new TreeLeaf();
		} else {
			children[index] = new TreeNode(this, index);
		}
	}
	return children[index];
}

#endif /* SRC_TREE_H_ */
