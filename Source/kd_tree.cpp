#include "kd_tree.h"

KDNode* KDNode::build (vector<Triangle>& triangles) const {
	KDNode* node = new KDNode();
	node->triangles = triangles;
	node->left = NULL;
	node->right = NULL;
	//node->bbox = BoundingBox();
	if(triangles.size() == 0) 
		return node;

	//node-> bbox = triangles[0] -> GetBoundingBox();
	
	if(triangles.size() == 1) {
		return node;
	}
	return node;
}