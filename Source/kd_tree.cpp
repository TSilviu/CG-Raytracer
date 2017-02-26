#include "kd_tree.h"

KDNode* KDNode::build (vector<Triangle>& triangles) const {
	KDNode* node = new KDNode();
	node->triangles = triangles;
	node->left = NULL;
	node->right = NULL;
	//node->bbox = BoundingBox();
	if(triangles.size() == 0) 
		return node;

	//node-> bbox = BoundingBox(triangles);
	if(triangles.size() == 1) {
		node -> left = new KDNode();
		node -> right = new KDNode();
		//Nodes will be empty and will trigger the first basecase
		node -> left -> triangles = vector<Triangle>();
		node -> right -> triangles = vector<Triangle>();
		return node;
	}


	return node;
}