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
	vector<Triangle*> triangles;
	KDNode();
	KDNode* build (vector<Triangle*> triangle) const;
};

#endif