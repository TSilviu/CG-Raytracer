#ifndef SKELETON_H
#define SKELETON_H

#include "TestModel.h"
/* STRUCTURES 								*/
struct Intersection
{
    glm::vec3 position;
    float distance;
    int triangleIndex;
    glm::vec3 color;
};

int depthT = 0;
#endif