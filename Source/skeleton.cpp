#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <limits.h>
#include <X11/Xlib.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "ObjLoader.h"

using namespace std;

using glm::vec3;
using glm::vec2;
using glm::mat3;

#define CORNELL_BOX
#define TEXTURES_CIMG

#ifdef TEXTURES_CIMG
	#define cimg_use_jpeg
	#include "CImg/CImg.h"
	using namespace cimg_library;
	CImg<unsigned char> image;
	struct Texture {
		int height, width;
		vec3* pixels;
	};
	void LoadTexture(const char* texture_file, 	CImg<unsigned char>& image);
	vec3 pixelFromTexture(vec2 pos, Texture& texture);
	void ConvertCImg(CImg<unsigned char>& image, Texture& texture);
	Texture texture, nMap;
#endif

//#define TEXTURES_SDL
#ifdef TEXTURES_SDL
	#include "SDL_image.h"
#endif

#define RotationSpeed 0.05f	//Camera rotation speed
#define MoveSpeed 0.2f

#define LightMoveSpeed 0.2f
#define AntiAliasingSamples 4 //For more than 4, the jitter matrix has to be changed
#define DOFSamples 0
#define Aperture 1.0f
#define SoftShadowsSamples 1
#define ReflexionDepth 1

#define kEpsilon 0.0001
/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

//Camera stuff
float f = 3.0f;
float yaw = 0.0f;
float focal_depth = 4.0f;
mat3 cameraR;
vec3 camera(0.0f,0.0f, -4.0f);
vec3 focusPoint(0.0f, 0.0f, 0.0f);

//Light stuff
const float lightL = 0.2f;  //Square light length
vec3 lightPos( 0.0f, -0.5f, -0.7f );  //Centre of the light
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
vec3 background(1.0f, 1.0f, 1.0f);


//http://computergraphics.stackexchange.com/questions/4248/how-is-anti-aliasing-implemented-in-ray-tracing
float jitterMatrix[4 * 2] = {
    -1.0/4.0,  3.0/4.0,
     3.0/4.0,  1.0/3.0,
    -3.0/4.0, -1.0/4.0,
     1.0/4.0, -3.0/4.0,
};
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
bool ClosestIntersection(const vec3 start, const vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
vec3 DirectLight( const Intersection& i, const vector<Triangle>& triangles );
void ApplyDOF(int x, int y, vec3& color, const vector<Triangle>& triangles, Intersection& inter);
void ApplyAntiAliasing(int x, int y, vec3& color, const vector<Triangle>& triangles, Intersection& inter);
vec3 ApplyReflexions(const vector<Triangle>& triangles, const vec3 dir, Intersection inter, vec3 color);
vec3 reflect(const vec3& I, const vec3& N);
vec2 barycentricCoordinates(Triangle t, vec3 p);
mat3 perturbedNormal(Triangle t);

int main( int argc, char* argv[] )
{
	#ifdef TEXTURES_CIMG
		const char* texture_path = "Textures/text1.jpg";
		const char* nMap_path = "Textures/text1n.jpg";
		LoadTexture(texture_path, image);
		ConvertCImg(image, texture);
		LoadTexture(nMap_path, image);
		ConvertCImg(image, nMap);
	#endif

	vector<Triangle> triangles;

	#ifdef CORNELL_BOX
		LoadTestModel(triangles);
	#else 
		char const* filename = "Models/LowPolyBody.obj";
		if(LoadObject(triangles, filename)) {
			printf("Model Loaded succesfuly\n");
		} else return 0;
	#endif

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
		camera += MoveSpeed*forward; //Forwards
    }
    if( keystate[SDLK_DOWN] )
    {
		camera-= MoveSpeed*forward;  //Backwards
    }
    if( keystate[SDLK_j] )
    {
		camera -= MoveSpeed*right;  //Left
    }
    if( keystate[SDLK_l] ) {
		camera += MoveSpeed*right;  //Right
	}
    if( keystate[SDLK_i] )
    {
		camera -= MoveSpeed*down;  //Down
    }
    if( keystate[SDLK_k] ) {
		camera += MoveSpeed*down;  //Up
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

	//Control lights WASD
    if( keystate[SDLK_a] )
    {
		lightPos.x -= LightMoveSpeed;	//Left
    }
    if( keystate[SDLK_d] ) {
		lightPos.x += LightMoveSpeed;	//Right
	}
    if( keystate[SDLK_w] )
    {
		lightPos.y -= LightMoveSpeed;	//Up
    }
    if( keystate[SDLK_s] ) {
		lightPos.y += LightMoveSpeed;	//Down
	}

	//Change focal_depth
	if( keystate[SDLK_1] )
    {
		focal_depth -= 1.0f;
		printf("Focal depth changed to %f\n", focal_depth);
    }
    if( keystate[SDLK_2] ) {
		focal_depth += 1.0f;
		printf("Focal depth changed to %f\n", focal_depth);
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
			vec3 color(0.0f, 0.0f, 0.0f);
			
			ApplyAntiAliasing(x, y, color, triangles, inter);
			ApplyDOF(x, y, color, triangles, inter);

			color /= ( DOFSamples + AntiAliasingSamples);
			PutPixelSDL( screen, x, y, color );
		}
	}


	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

//Not used anywhere, might be useful later
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

bool ClosestIntersection(const vec3 start, const vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	closestIntersection.distance = std::numeric_limits<float>::max();
	closestIntersection.triangleIndex = -1;
	for (uint i = 0; i < triangles.size(); ++i) {
		const vec3 v0 = triangles[i].v0;
		const vec3 e1 = triangles[i].e1;				//Edge1
		const vec3 e2 = triangles[i].e2;				//Edge2
		const vec3 tvec = start - v0;

		const vec3 pvec = cross(dir, e2);
		const float det = dot(e1, pvec);

		if (abs(det) < kEpsilon) continue;
		const float invDet = 1.0f / det; 

		const float u = dot(tvec, pvec) * invDet; 
		if (u < 0.0f || u > 1.0f) continue; 
		const vec3 qvec = cross(tvec, e1);
		const float v = dot(dir, qvec) * invDet;
		if (v < 0.0f || u + v > 1.0f) continue;
		const float t = dot(e2, qvec) * invDet;

		if(t>=0.0f && closestIntersection.distance > t) {
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
	#ifdef TEXTURES_CIMG
		vec2 bary_coords = barycentricCoordinates(triangles[i.triangleIndex], i.position);
		vec3 file_normal = pixelFromTexture(bary_coords, nMap)*2.0f - 1.0f;
		vec3 n = normalize(perturbedNormal(triangles[i.triangleIndex]) * file_normal);		//The triangle's normal
	#else
		vec3 n = triangles[i.triangleIndex].normal;
	#endif
	vec3 average = vec3(0.f,0.f,0.f);
	Intersection objToLight;
	for(int sample = 0; sample < SoftShadowsSamples; ++sample) {
		float rand_x, rand_z;
		if(SoftShadowsSamples != 1) {//Use the center of the light if soft shadows are not used
			rand_x = ((((float)(rand() % RAND_MAX) / RAND_MAX) * 2.0f - 1.0f)) * lightL/2.f;
	    	rand_z = ((((float)(rand() % RAND_MAX) / RAND_MAX) * 2.0f - 1.0f)) * lightL/2.f;
	    } else rand_x = rand_z = 0.0f;

    	vec3 ray_destination(lightPos.x + rand_x, lightPos.y, lightPos.z + rand_z);
    	vec3 r = normalize(ray_destination - i.position);

		float radius = glm::distance(ray_destination, i.position);	//Distance |lightPos-i.position|
		vec3 B = lightColor / (float) (4.0f*M_PI*radius*radius);
		float aux = max(dot(r, n), 0.0f);
		vec3 D = B*aux;

		if(ClosestIntersection(i.position+r*0.0001f, r, triangles, objToLight))
			if(objToLight.distance < radius)
				average += vec3(0.f, 0.f, 0.f);
			else average += D;
		else average += D;
	}
	return average/(float) SoftShadowsSamples;
}

/*
//The camera model now also takes into account the aperture
//This allows the generation of different DOFs by shooting rays through a pixel
//from different starting positions
//http://stackoverflow.com/questions/13532947/references-for-depth-of-field-implementation-in-a-raytracer
*/
void ApplyDOF(int x, int y, vec3& color, const vector<Triangle>& triangles, Intersection& inter) {

	const float x_axis = (x - SCREEN_WIDTH/2.0f)/(SCREEN_WIDTH/2.0);
	const float y_axis = (y - SCREEN_HEIGHT/2.0f)/(SCREEN_HEIGHT/2.0);

	for( int s=0; s<DOFSamples; ++s) {
		float aperture_x = (((float)(rand() % RAND_MAX) / RAND_MAX) * 2.0f - 1.0f)/10.0f;
        float aperture_y = (((float)(rand() % RAND_MAX) / RAND_MAX) * 2.0f - 1.0f)/10.0f;
        vec3 cameraOffset(aperture_x, aperture_y, 0.0f);
		const vec3 dir(x_axis, y_axis, f);

		vec3 C = camera + focal_depth* normalize(dir);
		vec3 ray_dir = C - (camera+cameraOffset);
		
		if(ClosestIntersection(camera + cameraOffset, cameraR*ray_dir, triangles, inter)) {
			vec3 directLight = DirectLight(inter, triangles);
			color += triangles[inter.triangleIndex].color*(indirectLight + directLight);
		} else {
			color += vec3(1.0f, 1.0f, 1.0f);
		}
	}
}

vec3 ApplyReflexions(const vector<Triangle>& triangles, const vec3 dir, Intersection inter, vec3 color, int depth) {
	if(triangles[inter.triangleIndex].reflective == 1 && depth < ReflexionDepth) {
    	vec3 r = normalize(dir);
		Intersection objToIntersection;
		vec3 R = reflect(r, triangles[inter.triangleIndex].normal);
		vec3 r_r = normalize(R);
		if (ClosestIntersection(inter.position+r_r*0.0001f, r_r, triangles, objToIntersection)) {
			return ApplyReflexions(triangles, r_r, objToIntersection, color, ++depth)*triangles[objToIntersection.triangleIndex].color;
		} 
	}
	return color;
}

/*
//Shoots multiple rays from the camera to a single pixel(same start, slightly different direction)
//Colects all of the colours for the pixels and averages them at the end, resulting in "soft edges"
*/
void ApplyAntiAliasing(int x, int y, vec3& color, const vector<Triangle>& triangles, Intersection& inter){
	for(int sample = 0; sample <AntiAliasingSamples; ++sample) {
		const float x_axis = (x - SCREEN_WIDTH/2.0f + jitterMatrix[2*sample])/(SCREEN_WIDTH/2.0);
		const float y_axis = (y - SCREEN_HEIGHT/2.0f + jitterMatrix[2*sample + 1])/(SCREEN_HEIGHT/2.0);
		const vec3 dir(x_axis, y_axis, f);

		if(ClosestIntersection(camera, cameraR*dir, triangles, inter)) {
			vec3 directLight = DirectLight(inter, triangles);
			#ifdef TEXTURES_CIMG 
				vec2 bary_coords = barycentricCoordinates(triangles[inter.triangleIndex], inter.position);
				vec3 texture_color = pixelFromTexture(bary_coords, texture);
			#else
				vec3 texture_color = triangles[inter.triangleIndex].color;
			#endif
		    vec3 color_reflections = ApplyReflexions(triangles, dir, inter, texture_color, 0);
			color += color_reflections*(indirectLight + directLight);

		} else {
			color += vec3(1.0f, 1.0f, 1.0f);
		}
	}
}

vec3 reflect(const vec3& I, const vec3& N){
	float c1 = -dot( I, N );
	return I + (2.0f * N * c1 );
}

mat3 perturbedNormal(Triangle t) {

        // Shortcuts for vertices
        vec3 v0 = t.v0;
        vec3 v1 = t.v1;
        vec3 v2 = t.v2;

        // Shortcuts for UVs
        vec2 uv0 = t.uv0;
        vec2 uv1 = t.uv1;
        vec2 uv2 = t.uv2;

        // Edges of the triangle : postion delta
        vec3 deltaPos1 = v1-v0;
        vec3 deltaPos2 = v2-v0;

        // UV delta
        vec2 deltaUV1 = uv1-uv0;
        vec2 deltaUV2 = uv2-uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
        vec3 normal = t.normal;
        return mat3(tangent, bitangent, normal);
}

vec2 barycentricCoordinates(Triangle t, vec3 p) {

	vec3 a = t.v0;
	vec3 b = t.v1;
	vec3 c = t.v2;

	//Just for consistency
	vec2 auv = t.uv0;
	vec2 buv = t.uv1;
	vec2 cuv = t.uv2;

	float areaABC = dot( t.normal, cross( (b - a), (c - a) )  ) ;
	float areaPBC = dot( t.normal, cross( (b - p), (c - p) )  ) ;
	float areaPCA = dot( t.normal, cross( (c - p), (a - p) )  ) ;

	vec3 bary;

	bary.x = areaPBC / areaABC ; // alpha
	bary.y = areaPCA / areaABC ; // beta
	bary.z = 1.0f - bary.x - bary.y ; // gamma

	return bary.x*auv + bary.y*buv + bary.z*cuv;
}

#ifdef TEXTURES_CIMG
vec3 pixelFromTexture(vec2 pos, Texture& texture) {
	int img_x = (int) (pos.x*texture.width);
	int img_y = (int) (pos.y*texture.height);
	return texture.pixels[img_y*texture.width + img_x];//vec3(r, g, b);
}


void LoadTexture(const char* texture_file, 	CImg<unsigned char>& image) {
	image = CImg<unsigned char>(texture_file);
	//TODO: Should add some error handling :) 
}

void ConvertCImg(CImg<unsigned char>& image, Texture& texture) {
	texture.width = image.width();
	texture.height = image.height();
	texture.pixels = new vec3[texture.height*texture.width];
	for(int i=0; i<texture.height; i++)
		for(int j=0; j<texture.width; j++) {
			float r = (float) image(j, i, 0, 0)/255.f;
			float g = (float) image(j, i, 0, 1)/255.f;
			float b = (float) image(j, i, 0, 2)/255.f;
			texture.pixels[(texture.height-i-1) * texture.width + j] = vec3(r, g, b);
		}
	//delete &image;
}
#endif



