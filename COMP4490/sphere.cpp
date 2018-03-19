#include "Sphere.h"


// SPHERE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Sphere::Sphere(void) {
	pos = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _pos, glm::vec3 _colour, float _radius) {
	pos = _pos;
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
			float thc = glm::sqrt((radius*radius) - ((glm::dot(L, L)) - (tca * tca))); // Thc = sqrt( radius^2 -  d^2 )using pythagorean algorithm
			float t0 = tca - thc;
			*t = t0;  // t0 = Tca - Thc -> The point where the ray intersects with the sphere
			return true;
		}
	}
}


glm::vec3 Sphere::centroid() {
	return this->pos;
}

glm::vec3 Sphere::calNormal(glm::vec3 _p0) {
	N = glm::normalize(_p0 - pos);
	return N;
}

void Sphere::getSurfaceData(glm::vec3 p0, glm::vec3 *_N, glm::vec2 *tex) {
	*_N = p0 - this->pos;
	*_N = glm::normalize(*_N);
	tex->x = (1 + atan2(_N->z, _N->x) / (glm::atan(1) * 4)) * 0.5;
	tex->y = acosf(_N->y) / (glm::atan(1));
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Sphere::getMaxX() { return this->pos.x + this->radius; }
float Sphere::getMaxY() { return this->pos.y + this->radius; }
float Sphere::getMaxZ() { return this->pos.z + this->radius; }
float Sphere::getMinX() { return this->pos.x - this->radius; }
float Sphere::getMinY() { return this->pos.y - this->radius; }
float Sphere::getMinZ() { return this->pos.z - this->radius; }
