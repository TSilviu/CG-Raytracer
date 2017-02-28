#ifndef KDTREE_H
#define KDTREE_H

#include "TestModel.h"
#include "BoundingBox.h"
#include <iostream>

using namespace std;

class KDNode {
public:
	BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	vector<Triangle> triangles;
	KDNode();
	KDNode* build(vector<Triangle> triangles, int depth) const;
	bool traverse(KDNode* root, glm::vec3 r_orig, glm::vec3 r_dir);
	//bool is leaf?
};

#endif