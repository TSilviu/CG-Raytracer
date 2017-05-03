#include "kd_tree.h"

glm::vec3 Midpoint(Triangle t) {
	glm::vec3 midpoint;

	float aux_min = std::min(std::min(t.v0.x, t.v1.x), t.v2.x);
	float aux_max = std::max(std::max(t.v0.x, t.v1.x), t.v2.x);
	midpoint.x = aux_min + (aux_max - aux_min)/2.0f;

	aux_min = std::min(std::min(t.v0.y, t.v1.y), t.v2.y);
	aux_max = std::max(std::max(t.v0.y, t.v1.y), t.v2.y);
	midpoint.y = aux_min + (aux_max - aux_min)/2.0f;

	aux_min = std::min(std::min(t.v0.z, t.v1.z), t.v2.z);
	aux_max = std::max(std::max(t.v0.z, t.v1.z), t.v2.z);
	midpoint.z = aux_min + (aux_max - aux_min)/2.0f;

	return midpoint;
}

bool Match(Triangle t0, Triangle t1) {
	if(t0.v0 == t1.v0 && t0.v1 == t1.v1 && t0.v2 == t1.v2) 
		return true;
	return false;
}

KDNode::KDNode() {};
KDNode* KDNode::build(vector<Triangle>& triangles, int depth)  {
	KDNode* node = new KDNode();
	node->is_leaf = true;
	node->triangles = triangles;
	node->left = NULL;
	node->right = NULL;
	node->bbox = BoundingBox();

	if(triangles.size() == 0)  {
		return node;
	}

	node -> bbox = BoundingBox(triangles);
	if(triangles.size() < 2 || depth > 5) {
		return node;
	}

	//Compute midpoint for all triangles:
	glm::vec3 midpoint(0.f, 0.f, 0.f);
	for(int i=0; i<triangles.size(); i++) {
		midpoint += Midpoint(triangles[i]);
	}
	midpoint *= 1.0f/triangles.size();

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
			if(Match(right_triangles[i], left_triangles[j])) matches++;
			//if(&right_triangles[i] == &left_triangles[j]) matches++;
		}
	
	if((float)matches/left_triangles.size() < 0.5f && (float) matches/right_triangles.size() < 0.5f) {
		node->is_leaf = false;
		node -> left = build(left_triangles, depth+1);
		node -> right = build(right_triangles, depth+1);
	}

	return node;
}

// #ifdef KDTREES
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
			closestIntersection.color = triangles[i].color;
		}
	}
	if(closestIntersection.triangleIndex == -1) {
		return false;
	}
	return true;
}
// #endif

void KDNode::output(KDNode* node) {
	if(node != NULL) {
		for (uint i = 0; i < node->triangles.size(); ++i) {
			cout<<"T"<<endl;
		}
		if(node->left != NULL) {
			node->output(node->left);
		}
		if(node->right != NULL) {
			node->output(node->right);
		}
	} else {cout<<"Should not get here"<<endl;}
}

// bool KDNode::traverse(KDNode* node, glm::vec3 r_orig, glm::vec3 r_dir, Intersection& inter, int depth) {
// 	if(node == NULL) return false;

// 	if(!bbox.Hit(r_orig, r_dir)) return false;
// 	else { cout<<node->triangles.size()<<endl; }

// 	// cout<<"hello from here"<<endl;
// 	if(node->is_leaf) { //Is leaf?
// 	//depthT++;
// 	// cout<<"Got here, depth: "<< depth <<" "<< node->triangles.size()<<endl;
// 		Intersection i;
// 		if(ClosestIntersection(r_orig, r_dir, node->triangles, i)) {
// 			if(i.distance < inter.distance)  {
// 				inter = i;
// 				return true;
// 			}
// 		else 
// 			return false;
// 		}
// 	}

// 	// If one of the nodes still has children:
// 	// depthT++;
// 	// cout<<"Got here, depth: "<< depthT <<endl;

// 	// Intersection i1, i2;m
// 	bool right_trav = this -> traverse(node->right, r_orig, r_dir, inter, depth + 1);
// 	bool left_trav = this -> traverse(node->left, r_orig, r_dir, inter, depth + 1);

// 	return  right_trav || left_trav; 
// }

bool KDNode::traverse(KDNode* node, glm::vec3 r_orig, glm::vec3 r_dir, Intersection& inter, int depth) {
	std::vector<StackItem> stack;

	if(node == NULL) return false;

	inter.distance = std::numeric_limits<float>::max();
	inter.triangleIndex = -1;

	float maxt;
	if(!node->bbox.Hit(r_orig, r_dir, maxt)) return false;

	StackItem item;
	
	float tr, tl;
	maxt = std::numeric_limits<float>::max();
	while(1) {
		if(!node->is_leaf) {
			bool left_hit = node->left->bbox.Hit(r_orig, r_dir, tl);
			bool right_hit = node->right->bbox.Hit(r_orig, r_dir, tr);
			// cout<<tl<< " "<<tr<< endl;
			if(left_hit && right_hit) {
				//push the node with the larger t value first;
				if(tr > tl) {
					//if(tl < maxt) {
						item.node = node->left;
						item.t = tl;
						stack.push_back(item);
					//}

					node = node -> right;
					continue;
				}
				else {
					item.node = node->right;
					item.t = tr;
					stack.push_back(item);

					node = node -> left;
					continue;
				}

			} else if(left_hit && tl < maxt) {
				node = node->left;
				continue;
			} else if(right_hit && tr < maxt) {
				node = node->right;
				continue;
			}
		}
		else {
			Intersection i;
			if(ClosestIntersection(r_orig, r_dir, node->triangles, i)) {
				if(i.distance < inter.distance) {
					maxt = inter.distance;
					inter = i;
				}
			}
		}
		if(stack.empty()) {
			if(inter.triangleIndex != -1) return true;
			else return false;
		}
		else{ 
			node = stack.back().node;
			stack.pop_back();
			// node = stack.top().node;
			// stack.pop();
		}
	}
	if(inter.triangleIndex != -1) return true;

	return  false; 
}


