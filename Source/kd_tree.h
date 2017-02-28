#ifndef KDTREE_H
#define KDTREE_H

#include "TestModel.h"
#include "BoundingBox.h"
#include "skeleton.h"
#include <iostream>

using namespace std;
bool ClosestIntersection(const glm::vec3 start, const glm::vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
class KDNode {
public:
	BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	vector<Triangle> triangles;
	KDNode();
	KDNode* build(vector<Triangle> triangles, int depth) const;
	bool traverse(KDNode* node, glm::vec3 r_orig, glm::vec3 r_dir,  Intersection& inter);
	//bool is leaf?
};

#endif