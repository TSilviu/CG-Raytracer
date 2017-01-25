#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

/* ----------------------------------------------------------------------------*/

/* STRUCTURES 								*/
struct Intersection
{
    vec3 position;
    float distance;
    int triangleIndex;
};

/* FUNCTIONS                                                                   */

void Update();
void Draw();
void Interpolate( float a, float b, vector<float>& result );
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );

vector<vec3> stars( 1000 );
int main( int argc, char* argv[] )
{
	vector<Triangle> triangles;
	LoadTestModel(triangles);

	for(int i=0; i<stars.size(); i++) {
		stars[i].x = float(rand()) / float(RAND_MAX) * (float)(rand()>RAND_MAX/2? -1.0:1.0);
		stars[i].y = float(rand()) / float(RAND_MAX) * (float)(rand()>RAND_MAX/2? -1.0:1.0);
		stars[i].z = float(rand()) / float(RAND_MAX);
		cout<<stars[i].x<<" "<<stars[i].y<<"\n";
	}

	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	for( int s=0; s<stars.size(); ++s )
	{
	    stars[s].z = stars[s].z - 0.01;
	    if( stars[s].z <= 0 )
	stars[s].z += 1;
	    if( stars[s].z > 1 )
	stars[s].z -= 1;
	}
}

bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	closestIntersection.distance = std::numeric_limits<float>::max();
	closestIntersection.triangleIndex = -1;
	for (int i = 0; i<triangles.size(); ++i) {
		vec3 v0 = triangles[i].v0;
		vec3 v1 = triangles[i].v1;
		vec3 v2 = triangles[i].v2;
		vec3 e1 = v1 - v0;				//Edge1
		vec3 e2 = v2 - v0;				//Edge2
		vec3 b = start - v0;
		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b; //Intersection point 

		const float t = x.x;
		const float u = x.y;
		const float v = x.z;

		if(u>0 && v>0 && u+v<1 && t>=0) {
			closestIntersection.position = v0 + u*e1 + u*e2;
			closestIntersection.distance = t; 
			closestIntersection.triangleIndex = i; 
			printf("Yuhu, new closest intersected\n");
		}
	}
	if(closestIntersection.triangleIndex == -1) {
		return false;
	}
	return true;
}

void Interpolate( vec3 a, vec3 b, vector<vec3>& result ) {
	const int size = result.size();
	float step_x = (b.x-a.x)/float(size-1);
	float step_y = (b.y-a.y)/float(size-1);
	float step_z = (b.z-a.z)/float(size-1);
	float x = step_x>=0? min(a.x, b.x): max(a.x, b.x);
	float y = step_y>=0? min(a.y, b.y): max(a.y, b.y);
	float z = step_z>=0? min(a.z, b.z): max(a.z, b.z);
	//if(result.size() > 1) 
	for( int i=0; i<size; ++i ) {
		result[i].x = x + step_x*i;
		result[i].y = y + step_y*i;
		result[i].z = z + step_z*i;
	}
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	//INTERPOLATION
	// vec3 topLeft(1,0,1);     // red
	// vec3 topRight(0,0,1);    // blue
	// vec3 bottomRight(0,1,0); // green
	// vec3 bottomLeft(1,1,0);  // yellow

	// vector<vec3> leftSide( SCREEN_HEIGHT );
	// vector<vec3> rightSide( SCREEN_HEIGHT );
	// Interpolate( topLeft, bottomLeft, leftSide );
	// Interpolate( topRight, bottomRight, rightSide );

	// for( int y=0; y<SCREEN_HEIGHT; ++y )
	// {
	// 	vector<vec3> center( SCREEN_WIDTH );
	// 	Interpolate( leftSide[y], rightSide[y], center);
	// 	for( int x=0; x<SCREEN_WIDTH; ++x )
	// 	{
	// 		PutPixelSDL( screen, x, y, center[x] );
	// 	}
	// }
	d = (x − W/2, y − W/2, f )
	for(int s=0; s<stars.size(); ++s) {
		float f = SCREEN_HEIGHT/2;
		vec3 color = 0.2f * vec3(1,1,0) / (stars[s].z*stars[s].z);
		PutPixelSDL( screen, f*(stars[s].x/stars[s].z) + SCREEN_WIDTH/2, f*(stars[s].y/stars[s].z) + f, color);
	}


	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
