#include "TestModel.h"

using namespace std;

class KDNode {
public:
	//BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	vector<Triangle> triangles;
	KDNode();
	KDNode* build (vector<Triangle>& triangle) const;
};
