#ifndef OBJ_FILE_LOADER
#define OBJ_FILE_LOADER

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include "TestModel.h"


bool LoadObject(std::vector<Triangle>& triangles, const char* filename) {
	FILE * file = fopen(filename, "r");

	if( file == NULL ){
	    printf("Impossible to open the file  %d!\n", errno);
	    return false;
	}

	glm::vec3 colour(0.5f, 0.5f, 0.5f);
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvcoords; 
	int reflection = 0;
	while( 1 ) {
	    char lineHeader[256];
	    // read the first word of the line
	    int res = fscanf(file, "%s", lineHeader);
	    if (res == EOF) {
	        break; 
	    }
	    if(strcmp( lineHeader, "vn" ) == 0) {
	    	glm::vec3 normal;
	    	fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
	    	normals.push_back(normal);
	    } else if (strcmp( lineHeader, "vt" ) == 0 ) { 
	    	glm::vec2 uvcoord;
	    	fscanf(file, "%f %f\n", &uvcoord.x, &uvcoord.y );
	    	uvcoords.push_back(uvcoord);
	    } else if ( strcmp( lineHeader, "v" ) == 0 ) {
		    glm::vec3 vertex;
		    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
		    vertices.push_back(vertex);
		} else if ( strcmp( lineHeader, "f" ) == 0 ) {
			unsigned int vertexIndex[3], uvIndex[3];
			int no_reads = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0],  &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2] );
			if(no_reads > 6) {
				printf("Too much information in the f line\n");
				return false;
			}
			// -1 because obj indexes start from 1
			Triangle t = Triangle(vertices[vertexIndex[0] - 1], vertices[vertexIndex[1] - 1], vertices[vertexIndex[2] - 1], colour, reflection);
			// t.SetNormal(normal[verte])
			t.SetUV(uvcoords[uvIndex[0] - 1], uvcoords[uvIndex[1] - 1], uvcoords[uvIndex[2] - 1]);
			triangles.push_back(t);
		}
	}

	for(int i=0; i<vertices.size(); i++) {

	}
	
	return true;
}

#endif