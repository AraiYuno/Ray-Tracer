#include <stdio.h>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "FL\OBJ_Loader.h"
#include "a1main.h"

using namespace std;

void trace(char *input_file, void *window, int width, int height) {
	renderSI(window, width, height);             // Rendering Sphere Intersection
}

void pick(void *window, int x, int y) {
	unsigned char r, g, b;
	if (get(window, x, y, r, g, b)) {
		printf("pick @ (%d, %d): colour [%d %d %d]\n", x, y, r, g, b);
		set(window, x, y, (13+r)%256, (25+g)%256, (41+b)%256);
		redraw(window);
	}
}

//=================================================================================
//renderSI
//  renders sphere intersection using raytracing.
//
//=================================================================================
void renderSI(void * window, int width, int height)
{
	Mesh* meshArr[10];
	meshArr[0] = new Sphere(glm::vec3(0, -2, -15), glm::vec3(255, 0, 0), 1); // RED SHPERE
	meshArr[1] = new Sphere(glm::vec3(2, 0, -15), glm::vec3(0, 255, 0), 1.5); // BLUE SPHERE
	meshArr[2] = new Sphere(glm::vec3(4, 0, -15), glm::vec3(0, 0, 255), 2); // GREEN SPHERE
	meshArr[3] = new Sphere(glm::vec3(0, -1, -16), glm::vec3(255, 255, 0), 2);  // YELLOW SHPERE

	// Triangular Mesh -> 3D. This is 4 sides triangle mesh with square at the bottom
	meshArr[4] = new Triangle(glm::vec3(0, -2, -15), glm::vec3(-2, 2, -13), glm::vec3(2, 2, -13), glm::vec3(128, 0.0, 128)); // PURPLE TRIANGLE
	meshArr[5] = new Triangle(glm::vec3(0, -2, -15), glm::vec3(2, 2, -13), glm::vec3(2, 2, -17), glm::vec3(128, 0.0, 128));
	meshArr[6] = new Triangle(glm::vec3(0, -2, -15), glm::vec3(2, 2, -17), glm::vec3(-2, 2, -17), glm::vec3(128, 0.0, 128));
	meshArr[7] = new Triangle(glm::vec3(0, -2, -15), glm::vec3(-2, 2, -17), glm::vec3(-2, 2, -13), glm::vec3(128, 0.0, 128));
	// bottom retangular
	meshArr[8] = new Triangle(glm::vec3(-2, 2, -17), glm::vec3(2, 2, -17), glm::vec3(2, 2, -13), glm::vec3(128, 0.0, 128));
	meshArr[9] = new Triangle(glm::vec3(-2, 2, -17), glm::vec3(-2, 2, -13), glm::vec3(2, 2, -13), glm::vec3(128, 0.0, 128));



	glm::vec3 **image = new glm::vec3*[width];
	for (int i = 0; i < width; i++) {
		image[i] = new glm::vec3[height];
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float pixRemapX = (2 * ((x + 0.5) / width) - 1)*(width/height) ;
			float pixRemapY = 1 - 2 * ((y + 0.5) / height);
			float pixCamX = pixRemapX * glm::tan(glm::radians(45.0) / 2);
			float pixCamY = pixRemapY * glm::tan(glm::radians(45.0) / 2);
			glm::vec3 PcamSpace = glm::vec3(pixCamX, pixCamY, -1);
			glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
			glm::vec3 rayDirection = glm::normalize(PcamSpace - rayOrigin);

			float minT = INFINITY;
			int sphereHit = -1;
			float t0 = 0.0f;

			for (int i = 0; i < sizeof(meshArr) / sizeof(meshArr[0]); i++) {
				bool hit = meshArr[i]->Intersection(rayOrigin, rayDirection, &t0);

				if (hit && t0 < minT) {
					minT = t0;
					sphereHit = i;
				}

				if (sphereHit != -1) {
					set(window, x, y, meshArr[sphereHit]->colour.r, meshArr[sphereHit]->colour.g,
						meshArr[sphereHit]->colour.b);
				}
				else {
					set(window, x, y, 255, 255, 255);
				}
			}
		}
	}
}

// MESH DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Mesh::Mesh(void) {
	pos = glm::vec3(0, 0, 0);
	colour = glm::vec3(0, 0, 0);
	N = glm::vec3(0, 0, 0);  // Normal
}

Mesh::Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N) {
	pos = _position;
	colour = _colour;
	N = _N;
}

bool Mesh::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	return false;
}

glm::vec3 Mesh::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	return N;
}



// SPHERE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Sphere::Sphere(void) {
	pos = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, glm::vec3 _colour, float _radius) {
	pos = _position;
	colour = _colour;
	radius = _radius;
}

//======================================================================================
// Intersection -> SPHPERE
//    returns true if the ray intersects with with a sphere
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
//    The link has the algorithm that I used.
//======================================================================================
bool Sphere::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	glm::vec3 L = pos - _rayOrigin;    // L = C - O
	float tca = glm::dot(L, _rayDirection);  // Tca = L scalar product D
	if (tca < 0) {		// if Tca < 0, the point is located behind the ray origin.
		return false;
	}
	else {
		float d = glm::sqrt((glm::dot(L, L)) - (tca * tca)); // d = L^2 - Tca^2.
		if (d > radius) { // if d > radius, the ray misses the sphere, and there is no intersection
			return false;
		} // if d <= radius, then the ray intersects with the sphere.
		else {
			float thc = glm::sqrt((radius*radius) - ((glm::dot(L, L)) - (tca * tca)) ); // Thc = sqrt( radius^2 -  d^2 )using pythagorean algorithm
			float t0 = tca - thc;
			*t = t0;  // t0 = Tca - Thc -> The point where the ray intersects with the sphere
			return true;
		}
	}
}

glm::vec3 Sphere::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	*_shininess = 120;  // to give a glow effect
	*_diffuse = colour;
	*_specular = glm::vec3(0.66, 0.66, 0.66);
	return (_p0 - pos);
}



// TRIANGLE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================

// Null constructor
Triangle::Triangle(void) {
	a = glm::vec3(0, 0, 0);
	b = glm::vec3(0, 0, 0);
	c = glm::vec3(0, 0, 0);
}

// Basic Constructor
Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _colour) {
	a = _a;
	b = _b;
	c = _c;
	colour = _colour;
}

//=======================================================================================
// Intersection
//   checks intersection of a triangle and returns true if it does.
//   I have used Möller–Trumbore intersection algorithm.
//=======================================================================================
bool Triangle::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	const float EPSILON = 0.0000001;
	float x, f, u, v;
	glm::vec3 h, s, q;

	glm::vec3 edge1 = b - a;
	glm::vec3 edge2 = c - a;

	h = glm::cross(_rayDirection, edge2);
	x = glm::dot(edge1, h);
	if (x > -EPSILON && x < EPSILON)
		return false;

	f = 1 / x;
	s = _rayOrigin - a;
	u = f * (glm::dot(s, h));
	if (u < 0.0 || u > 1.0)
		return false;

	q = glm::cross(s, edge1);
	v = f * (glm::dot(_rayDirection, q));
	if (v < 0.0 || u + v > 1.0)
		return false;

	// By now we can surely compute t* to check where the intersection point is on the line.
	float t0 = f * (glm::dot(edge2, q));
	if (t0 > EPSILON) {   // Ray intersection in this case
		float t0 = glm::dot(edge2, glm::cross((_rayOrigin - a), edge1)) / glm::dot(edge1, glm::cross(_rayDirection, edge2));
		*t = t0;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection
		return false;

	
	/*float u = glm::dot((_rayOrigin - a), glm::cross(_rayDirection, edge2)) / glm::dot(edge1, (glm::cross(_rayDirection, edge2)));
	float v = glm::dot(_rayDirection, (glm::cross((_rayOrigin - a), edge1)) / glm::dot(edge1, glm::cross(_rayDirection, edge2)));

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || u + v > 1) {
		return false;
	}
	else {
		float t0 = glm::dot(edge2, glm::cross((_rayOrigin - a), edge1)) / glm::dot(edge1, glm::cross(_rayDirection, edge2));
		*t = t0;
		return true;
	}*/
}

glm::vec3 Triangle::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	*_shininess = 90;
	*_diffuse = colour;
	*_specular = glm::vec3(0.66, 0.66, 0.66);
	return *_diffuse;
}