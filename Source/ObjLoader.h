#ifndef OBJ_FILE_LOADER
#define OBJ_FILE_LOADER

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include "TestModel.h"


bool LoadObject(std::vector<Triangle>& triangles, char* filename) {
	FILE * file = fopen(filename, "r");
	if( file == NULL ){
	    printf("Impossible to open the file !\n");
	    return false;
	}
	return true;
}

#endif