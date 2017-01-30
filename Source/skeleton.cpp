#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

#define RotationSpeed 0.05f
#define MoveSpeed 0.2f
#define LightMoveSpeed 0.2f

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vec3 camera(0.0f,0.0f, -4.0f);
float f = 3.0f;
float yaw = 0.0f;

mat3     cameraR;
vec3 lightPos( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
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
void Draw(const vector<Triangle>& triangles);
void Interpolate( float a, float b, vector<float>& result );
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
vec3 DirectLight( const Intersection& i, const vector<Triangle>& triangles );

int main( int argc, char* argv[] )
{

	vector<Triangle> triangles;
	LoadTestModel(triangles);

	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw(triangles);
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

    Uint8* keystate = SDL_GetKeyState( 0 );

    //Get camera direction
	vec3 right(cameraR[0][0], cameraR[0][1], cameraR[0][2]);
	vec3 down(cameraR[1][0], cameraR[1][1], cameraR[1][2]);
	vec3 forward(cameraR[2][0], cameraR[2][1], cameraR[2][2]);

	//Control camera
    if( keystate[SDLK_UP] )
    {
		camera += MoveSpeed*forward;
    }
    if( keystate[SDLK_DOWN] )
    {
		camera-= MoveSpeed*forward;
    }

    //Rotate on Y axis 
    if( keystate[SDLK_LEFT] )
    {
		yaw += RotationSpeed;
		cameraR = mat3(cos(yaw), 0, sin(yaw), 0, 1, 0, -sin(yaw), 0, cos(yaw));
    }

	if( keystate[SDLK_RIGHT] ) {
		yaw -= RotationSpeed;
		cameraR = mat3(cos(yaw), 0, sin(yaw), 0, 1, 0, -sin(yaw), 0, cos(yaw));
	}

    if( keystate[SDLK_j] )
    {
		camera -= MoveSpeed*right;
    }
    if( keystate[SDLK_l] ) {
		camera += MoveSpeed*right;
	}
    if( keystate[SDLK_i] )
    {
		camera -= MoveSpeed*down;
    }
    if( keystate[SDLK_k] ) {
		camera += MoveSpeed*down;
	}

	//Control lights WASD
    if( keystate[SDLK_a] )
    {
		lightPos.x -= LightMoveSpeed;
    }
    if( keystate[SDLK_d] ) {
		lightPos.x += LightMoveSpeed;
	}
    if( keystate[SDLK_w] )
    {
		lightPos.y -= LightMoveSpeed;
    }
    if( keystate[SDLK_s] ) {
		lightPos.y += LightMoveSpeed;
	}

}

bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	closestIntersection.distance = std::numeric_limits<float>::max();
	closestIntersection.triangleIndex = -1;
	for (uint i = 0; i<triangles.size(); ++i) {
		const vec3 v0 = triangles[i].v0;
		const vec3 v1 = triangles[i].v1;
		const vec3 v2 = triangles[i].v2;
		const vec3 e1 = v1 - v0;				//Edge1
		const vec3 e2 = v2 - v0;				//Edge2
		const vec3 b = start - v0;
		const mat3 A( -dir, e1, e2 );
		const vec3 x = glm::inverse( A ) * b; //Intersection point 

		const float t = x.x;
		const float u = x.y;
		const float v = x.z;


		if(t>=0 && closestIntersection.distance > t && u>=0 && v>=0 && (u+v)<=1) {
			closestIntersection.position = v0 + u*e1 + v*e2;
			closestIntersection.distance = t; 
			closestIntersection.triangleIndex = i; 
		}
	}
	if(closestIntersection.triangleIndex == -1) {
		return false;
	}
	return true;
}

vec3 DirectLight( const Intersection& i, const vector<Triangle>& triangles  ) {
	vec3 n = triangles[i.triangleIndex].normal;		//The triangle's normal									
	vec3 r = normalize(lightPos - i.position);  	//Unit vector from surface to light sphere

	float radius = distance(lightPos, i.position);	//Distance |lightPos-i.position|
	vec3 B = lightColor / (float) (4.0f*M_PI*radius*radius);	
	float aux = max(dot(r, n), 0.0f);
	vec3 D = B*aux;

	Intersection objToLight;
	if(ClosestIntersection(i.position, r, triangles, objToLight))
		if(objToLight.distance < radius)
			D = vec3(0.f, 0.f, 0.f); 
				
	return D;
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

void Draw(const vector<Triangle>& triangles)
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	#pragma omp parallel for //firstprivate(color)
	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{	
			Intersection inter;
			vec3 color;
			const float x_axis = (x - SCREEN_WIDTH/2.0f)/(SCREEN_WIDTH/2.0);
			const float y_axis = (y - SCREEN_HEIGHT/2.0f)/(SCREEN_HEIGHT/2.0);
			const vec3 dir(x_axis, y_axis, f);
			const vec3 start(x_axis, y_axis, camera.z);
			if(ClosestIntersection(camera, cameraR*dir, triangles, inter)) {
				vec3 directLight = DirectLight(inter, triangles);
				color = triangles[inter.triangleIndex].color*directLight;
			} else {
				color = vec3(1.0f, 1.0f, 1.0f);
			}
			PutPixelSDL( screen, x, y, color );
		}
	}


	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
