#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;
	int reflective;	//reflective property of triangle; can be either 0 or 1
	glm::vec3 uv0;
	glm::vec3 uv1;
	glm::vec3 uv2;

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, int reflective )
		: v0(v0), v1(v1), v2(v2), color(color), reflective(reflective)
	{
		ComputeNormal();
	}

	void SetUV(glm::vec3 uv0, glm::vec3 uv1, glm::vec3 uv2) {
		this->uv0 = uv0;
		this->uv1 = uv1;
		this->uv2 = uv2;
	}
	
	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross( e2, e1 ) );
	}
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<Triangle>& triangles )
{
	using glm::vec3;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );
	vec3 pure_white( 1.0f, 1.0f, 1.0f );
	triangles.clear();
	triangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);

	// Floor:
	triangles.push_back( Triangle( C, B, A, green, 0 ) );
	triangles.push_back( Triangle( C, D, B, green, 0 ) );

	// Left wall
	triangles.push_back( Triangle( A, E, C, pure_white, 1 ) );
	triangles.push_back( Triangle( C, E, G, pure_white, 1 ) );

	// Right wall
	triangles.push_back( Triangle( F, B, D, yellow, 0 ) );
	triangles.push_back( Triangle( H, F, D, yellow, 0 ) );

	// Ceiling
	triangles.push_back( Triangle( E, F, G, cyan, 0 ) );
	triangles.push_back( Triangle( F, H, G, cyan, 0 ) );

	// Back wall
	triangles.push_back( Triangle( G, D, C, white, 0 ) );
	triangles.push_back( Triangle( G, H, D, white, 0 ) );

	// ---------------------------------------------------------------------------
	// Short block

	A = vec3(290,0,114);
	B = vec3(130,0, 65);
	C = vec3(240,0,272);
	D = vec3( 82,0,225);

	E = vec3(290,165,114);
	F = vec3(130,165, 65);
	G = vec3(240,165,272);
	H = vec3( 82,165,225);

	// Front
	triangles.push_back( Triangle(E,B,A,red, 1) );
	triangles.push_back( Triangle(E,F,B,red, 1) );

	// Front
	triangles.push_back( Triangle(F,D,B,red, 1) );
	triangles.push_back( Triangle(F,H,D,red, 1) );

	// BACK
	triangles.push_back( Triangle(H,C,D,red, 1) );
	triangles.push_back( Triangle(H,G,C,red, 1) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,red, 1) );
	triangles.push_back( Triangle(E,A,C,red, 1) );

	// TOP
	triangles.push_back( Triangle(G,F,E,red, 1) );
	triangles.push_back( Triangle(G,H,F,red, 1) );

	// ---------------------------------------------------------------------------
	// Tall block

	A = vec3(423,0,247);
	B = vec3(265,0,296);
	C = vec3(472,0,406);
	D = vec3(314,0,456);

	E = vec3(423,330,247);
	F = vec3(265,330,296);
	G = vec3(472,330,406);
	H = vec3(314,330,456);

	// Front
	triangles.push_back( Triangle(E,B,A,blue, 1) );
	triangles.push_back( Triangle(E,F,B,blue, 1) );

	// Front
	triangles.push_back( Triangle(F,D,B,blue, 1) );
	triangles.push_back( Triangle(F,H,D,blue, 1) );

	// BACK
	triangles.push_back( Triangle(H,C,D,blue, 1) );
	triangles.push_back( Triangle(H,G,C,blue, 1) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,blue, 1) );
	triangles.push_back( Triangle(E,A,C,blue, 1) );

	// TOP
	triangles.push_back( Triangle(G,F,E,blue, 1) );
	triangles.push_back( Triangle(G,H,F,blue, 1) );


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<triangles.size(); ++i )
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec3(1,1,1);
		triangles[i].v1 -= vec3(1,1,1);
		triangles[i].v2 -= vec3(1,1,1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].ComputeNormal();
	}
}

#endif