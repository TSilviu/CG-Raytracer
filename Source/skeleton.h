#ifndef SKELETON_H
#define SKELETON_H

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include <X11/Xlib.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "ObjLoader.h"
#include "kd_tree.h"
#include "BoundingBox.h"
#include "BoundingBox.cpp"
#include "kd_tree.cpp"

/* STRUCTURES 								*/
struct Intersection
{
    glm::vec3 position;
    float distance;
    int triangleIndex;
};

#endif