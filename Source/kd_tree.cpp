#include "kd_tree.h"

glm::vec3 Midpoint(Triangle t) {
	glm::vec3 midpoint;
	midpoint.x = (t.v0.x + t.v1.x + t.v2.x)/3.0f;
	midpoint.y = (t.v0.y + t.v1.y + t.v2.y)/3.0f;
	midpoint.z = (t.v0.z + t.v1.z + t.v2.z)/3.0f;

	return midpoint;
}

KDNode::KDNode() {};
KDNode* KDNode::build(vector<Triangle> triangles, int depth) const {
	KDNode* node = new KDNode();
	node->triangles = triangles;
	node->left = NULL;
	node->right = NULL;
	node->bbox = BoundingBox();

	cout<<triangles.size()<<endl;
	if(triangles.size() == 0 || depth > 5) 
		return node;

	node -> bbox = BoundingBox(triangles);
	if(triangles.size() == 1) {
		node -> left = new KDNode();
		node -> right = new KDNode();
		//Nodes will be empty and will trigger the first basecase
		node -> left -> triangles = vector<Triangle>();
		node -> right -> triangles = vector<Triangle>();
		return node;
	}

	//Compute midpoint for all triangles:
	glm::vec3 midpoint;
	for(int i=0; i<triangles.size(); i++) {
		midpoint += Midpoint(triangles[i]);
	}
	midpoint /= triangles.size();

	//Partition tiriangles:
	std::vector<Triangle> left_triangles;
	std::vector<Triangle> right_triangles;

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
			if(&right_triangles[i] == &left_triangles[j]) matches++;
		}
	if((float)matches/left_triangles.size() < 0.5f && (float) matches/right_triangles.size() < 0.5f) {
		node -> left = build(left_triangles, depth+1);
		node -> right = build(right_triangles, depth+1);
	} else {
		node -> left = new KDNode();
		node -> right = new KDNode();
		node -> left -> triangles = std::vector<Triangle> ();
		node -> right -> triangles = std::vector<Triangle> ();
	}
	return node;
}





