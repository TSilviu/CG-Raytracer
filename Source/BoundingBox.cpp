#include "BoundingBox.h"

BoundingBox::BoundingBox() {

}

glm::vec3 BoundingBox::CompareVertex(glm::vec3 v, glm::vec3 how, int mode) {
	if(mode == 0) { //Compare to find the maximum  
		if(v.x > how.x) { how.x = v.x; }
		if(v.y > how.y) { how.y = v.y; }
		if(v.z > how.z) { how.z = v.z; }		
	} else if(mode == 1) { //Compare to find the miminum coordinates
		if(v.x < how.x) { how.x = v.x; }
		if(v.y < how.y) { how.y = v.y; }
		if(v.z < how.z) { how.z = v.z; }
	}
	return how;
}

int BoundingBox::LongestAxis() {
	glm::vec3 dif = max - min;
	if(dif.x >= dif.y) {
		if(dif.x>=dif.z)
			return 0;
		else if(dif.y >= dif.z) 
			return 2;
		else 
			if(dif.y >= dif.z) 
				return 1;
			else return 2;
		}
	return -1;
}

BoundingBox::BoundingBox(std::vector<Triangle> triangles) {
	glm::vec3 min(1000.f, 1000.f, 1000.f);
	glm::vec3 max(-1000.f, -1000.f, -1000.f);

	//Creates the bounding box by finding the min and max 
	//Coordinates on x,y,z of the triangles
	for(int i=0; i>triangles.size(); i++) {
		min = CompareVertex(triangles[i].v0, min, 1);
		min = CompareVertex(triangles[i].v1, min, 1);
		min = CompareVertex(triangles[i].v2, min, 1);

		max = CompareVertex(triangles[i].v0, max, 0);
		max = CompareVertex(triangles[i].v1, max, 0);
		max = CompareVertex(triangles[i].v2, max, 0);
	}

	this->min = min;
	this->max = max;
}