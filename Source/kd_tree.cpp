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

	if(triangles.size() == 0)  {
		return node;
	}

	cout<< depth<<endl;
	node -> bbox = BoundingBox(triangles);
	if(triangles.size() < 2 || depth > 5) {
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
	}

	return node;
}

bool ClosestIntersection(const glm::vec3 start, const glm::vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	closestIntersection.distance = std::numeric_limits<float>::max();
	closestIntersection.triangleIndex = -1;
	for (uint i = 0; i < triangles.size(); ++i) {
		const glm::vec3 v0 = triangles[i].v0;
		const glm::vec3 e1 = triangles[i].e1;				//Edge1
		const glm::vec3 e2 = triangles[i].e2;				//Edge2
		const glm::vec3 tvec = start - v0;

		const glm::vec3 pvec = cross(dir, e2);
		const float det = dot(e1, pvec);

		if (abs(det) < 0.0001f) continue;
		const float invDet = 1.0f / det; 

		const float u = dot(tvec, pvec) * invDet; 
		if (u < 0.0f || u > 1.0f) continue; 
		const glm::vec3 qvec = cross(tvec, e1);
		const float v = dot(dir, qvec) * invDet;
		if (v < 0.0f || u + v > 1.0f) continue;
		const float t = dot(e2, qvec) * invDet;

		if(t>=0.0f && closestIntersection.distance > t) {
			closestIntersection.position = v0 + u*e1 + v*e2;
			closestIntersection.distance = t;
			closestIntersection.triangleIndex = i;
		}
	}
	if(closestIntersection.triangleIndex == -1) {
		return false;
	}
	return true;
}

bool KDNode::traverse(KDNode* node, glm::vec3 r_orig, glm::vec3 r_dir, Intersection& inter) {

	if(node == NULL) return false;

	if(!bbox.Hit(r_orig, r_dir)) return false;

	if(node->left == NULL && node->right == NULL) { //Is leaf?
		return ClosestIntersection(r_orig, r_dir, node->triangles, inter);
	}

	// If one of the nodes still has children:
	bool left_trav = this -> traverse(node->left, r_orig, r_dir, inter);
	bool right_trav = false;
	if(!left_trav) right_trav = this -> traverse(node->right, r_orig, r_dir, inter);

	return false; 
}


