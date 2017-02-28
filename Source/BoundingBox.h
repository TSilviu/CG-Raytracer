#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "TestModel.h"
class BoundingBox {
public:   
	BoundingBox(std::vector<Triangle> triangles);
	BoundingBox();
	bool Hit(glm::vec3 r_orig, glm::vec3 r_dir);
	int LongestAxis();
	glm::vec3 CompareVertex(glm::vec3 v, glm::vec3 how, int mode);
	glm::vec3 min;
	glm::vec3 max;
};

#endif