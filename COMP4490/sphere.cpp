#include <iostream>
#include <glm/glm.hpp>
#include <stdio.h>
#include <fstream>

//#include "a1main.h"


using namespace std;


/*Sphere::Sphere(void) {
	position = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, float _radius, glm::vec3 _colour){
	position = _position;
	radius = _radius;
	colour = _colour;
}

bool Sphere::Intersection(float *t, glm::vec3 _rayOrigin, glm::vec3 _rayDirection) {
	glm::vec3 L = position - _rayOrigin;
	float tca = glm::dot(L, _rayDirection);
	if (tca < 0) {
		return false;
	}
	else {
		float s2 = (glm::dot(L, L)) - (tca * tca);
		float s = glm::sqrt(s2);
		if (s > radius) {
			return false;
		}
		else {
			float thc = glm::sqrt((radius*radius) - s2);
			float t0 = tca - thc;
			*t = t0;
			return true;
		}
	}
}*/