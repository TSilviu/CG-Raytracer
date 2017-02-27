#ifndef KDTREE_H
#define KDTREE_H

#include "TestModel.h"
#include "BoundingBox.h"
#include <iostream>

using namespace std;

class KDNode {
public:
	KDNode(){};
	KDNode* build(vector<Triangle> triangles, int depth) const;

	BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	vector<Triangle> triangles;
};

#endif