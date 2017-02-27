#include "kd_tree.h"

glm::vec3 Midpoint(Triangle* t) {

	return glm::vec3(0.f, 0.f, 0.f);
}

KDNode* KDNode::build (vector<Triangle*> triangles, int depth) const {
	KDNode* node = new KDNode();
	node->triangles = triangles;
	node->left = NULL;
	node->right = NULL;
	node->bbox = BoundingBox();
	if(triangles.size() == 0) 
		return node;

	node -> bbox = BoundingBox(triangles);
	if(triangles.size() == 1) {
		node -> left = new KDNode();
		node -> right = new KDNode();
		//Nodes will be empty and will trigger the first basecase
		node -> left -> triangles = vector<Triangle*>();
		node -> right -> triangles = vector<Triangle*>();
		return node;
	}

	//Compute midpoint for all triangles:
	glm::vec3 midpoint;
	for(int i=0; i<triangles.size(); i++) {
		midpoint += Midpoint(triangles[i]);
	}
	midpoint /= triangles.size();

	//Partition tiriangles:
	std::vector<Triangle*> left_triangles;
	std::vector<Triangle*> right_triangles;

	int axis = node -> bbox.LongestAxis();
	for(int i=0; i<triangles.size(); i++) {
		glm::vec3 mid = Midpoint(triangles[i]);
		switch(axis) {
			case 0: {
				midpoint.x >= mid.x ? right_triangles.push_back(triangles[i]):left_triangles.push_back(triangles[i]);
				break;
			}
			case 1: {
				midpoint.y >= mid.y ? right_triangles.push_back(triangles[i]):left_triangles.push_back(triangles[i]);
				break;
			}
			case 2: {
				midpoint.z >= mid.z ? right_triangles.push_back(triangles[i]):left_triangles.push_back(triangles[i]);
				break;
			}
		default: { cout<<"Error occured!"; }
		}
	}

	//Just so it fails the matches 
	if(left_triangles.size() == 0 && right_triangles.size()>0) left_triangles = right_triangles;
	if(right_triangles.size() == 0 && left_triangles.size()>0) right_triangles = left_triangles;

	int matches = 0;
	for(int i=0; i<right_triangles.size(); i++)
		for(int j=0; j<left_triangles.size(); j++) {
			if(right_triangles[i] == left_triangles[j]) matches++;
		}
	if((float)matches/left_triangles.size() < 0.5f && (float) matches/right_triangles.size() < 0.5f) {
		node -> left = build(left_triangles, depth+1);
		node -> right = build(right_triangles, depth+1);
	} else {
		node -> left = new KDNode();
		node -> right = new KDNode();
		node -> left -> triangles = std::vector<Triangle*> ();
		node -> right -> triangles = std::vector<Triangle*> ();
	}
	return node;
}





