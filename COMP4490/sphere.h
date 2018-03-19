#pragma once
#include "Mesh.h"

//======================================================================
// Sphere Intersection
//    Code for sphere intersection
//======================================================================
class Sphere : public Mesh {
public:
	float radius;
	Sphere(void);
	Sphere(glm::vec3 _pos, glm::vec3 _colour, float _radius);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 centroid();
	glm::vec3 calNormal(glm::vec3 _p0);
	void getSurfaceData(glm::vec3 p0, glm::vec3 *_N, glm::vec2 *tex);

	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
};

