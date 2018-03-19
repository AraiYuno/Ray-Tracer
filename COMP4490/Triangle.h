#pragma once
#include "Mesh.h"
//======================================================================
// Triangle Intersection
//    Code for triangle intersection
//======================================================================
class Triangle : public Mesh {
public:
	glm::vec3 a, b, c;
	float w, u, v;
	Triangle(void);
	Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _colour);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 centroid();
	glm::vec3 calNormal(glm::vec3 _p0);

	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
};