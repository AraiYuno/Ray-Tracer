//#include <iostream>
//#include <glm/glm.hpp>
//#include <stdio.h>
//#include <fstream>
//#include "sphere.h"
//
//using namespace std;
//
// SPHERE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
//Sphere::Sphere(void) {
//	pos = glm::vec3(0, 0, 0);
//	radius = 0;
//	colour = glm::vec3(0, 0, 0);
//}
//
//Sphere::Sphere(glm::vec3 _position, glm::vec3 _colour, float _radius) {
//	pos = _position;
//	colour = _colour;
//	radius = _radius;
//}
//
//======================================================================================
// Intersection -> SPHPERE
//    returns true if the ray intersects with with a sphere
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
//    The link has the algorithm that I used.
//======================================================================================
//bool Sphere::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
//	glm::vec3 L = pos - _rayOrigin;    // L = C - O
//	float tca = glm::dot(L, _rayDirection);  // Tca = L scalar product D
//	if (tca < 0) {		// if Tca < 0, the point is located behind the ray origin.
//		return false;
//	}
//	else {
//		float d = glm::sqrt((glm::dot(L, L)) - (tca * tca)); // d = L^2 - Tca^2.
//		if (d > radius) { // if d > radius, the ray misses the sphere, and there is no intersection
//			return false;
//		} // if d <= radius, then the ray intersects with the sphere.
//		else {
//			float thc = glm::sqrt((radius*radius) - ((glm::dot(L, L)) - (tca * tca))); // Thc = sqrt( radius^2 -  d^2 )using pythagorean algorithm
//			float t0 = tca - thc;
//			*t = t0;  // t0 = Tca - Thc -> The point where the ray intersects with the sphere
//			return true;
//		}
//	}
//}