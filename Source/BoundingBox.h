#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "TestModel.h"
class BoundingBox {
public:   
	BoundingBox(std::vector<Triangle*> triangles);
	BoundingBox();
	int LongestAxis();
	glm::vec3 CompareVertex(glm::vec3 v, glm::vec3 how, int mode);
	float x() { return pos.x; };
	float y() { return pos.y; };
	float z() { return pos.z; };
	float w() { return size.x; };
	float h() { return size.y; };
	float d() { return size.z; };
private:
	glm::vec3 pos;
	glm::vec3 size;
	glm::vec3 min;
	glm::vec3 max;
};

#endif