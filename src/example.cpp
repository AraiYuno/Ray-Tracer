#include <stdio.h>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

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
	Sphere spherArr[4];
	spherArr[0] = Sphere(glm::vec3(0, 0, -15), 1, glm::vec3(255, 0, 0)); // RED
	spherArr[1] = Sphere(glm::vec3(2, 0, -15), 1.5, glm::vec3(0, 255, 0)); // BLUE
	spherArr[2] = Sphere(glm::vec3(4, 0, -15), 2, glm::vec3(0, 0, 255)); // GREEN
	spherArr[3] = Sphere(glm::vec3(0, -1, -16), 2, glm::vec3(255, 255, 0));  // YELLOW

	glm::vec3 **image = new glm::vec3*[width];
	for (int i = 0; i < width; i++) {
		image[i] = new glm::vec3[height];
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float pixRemapX = (2 * ((x + 0.5) / width) - 1)*(width/height) ;
			float pixRemapY = 1 - 2 * ((y + 0.5) / height);
			float pixCamX = pixRemapX * glm::tan(glm::radians(30.0) / 2);
			float pixCamY = pixRemapY * glm::tan(glm::radians(30.0) / 2);
			glm::vec3 PcamSpace = glm::vec3(pixCamX, pixCamY, -1);
			glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
			glm::vec3 rayDirection = PcamSpace - rayOrigin;
			rayDirection = glm::normalize(rayDirection);

			float minT = INFINITY;
			int sphereHit = -1;
			float t0 = 0.0f;

			for (int i = 0; i < sizeof(spherArr) / sizeof(spherArr[0]); i++) {
				bool hit = spherArr[i].Intersection(&t0, rayOrigin, rayDirection);

				if (hit && t0 < minT) {
					minT = t0;
					sphereHit = i;
				}

				if (sphereHit != -1) {
					set(window, x, y, spherArr[sphereHit].colour.x, spherArr[sphereHit].colour.y,
						spherArr[sphereHit].colour.z);
				}
				else {
					set(window, x, y, 255, 255, 255);
				}
			}
		}
	}
}

Sphere::Sphere(void) {
	pos = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, float _radius, glm::vec3 _colour) {
	pos = _position;
	radius = _radius;
	colour = _colour;
}

//======================================================================================
// Intersection
//    returns true if the ray intersects with with a sphere
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
//    The link has the algorithm that I used.
//======================================================================================
bool Sphere::Intersection(float *t, glm::vec3 _rayOrigin, glm::vec3 _rayDirection) {
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