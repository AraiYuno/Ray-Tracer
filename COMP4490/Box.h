#pragma once
#include "Mesh.h"


//======================================================================
// Box Intersection
//    Code for Box Intersection
//======================================================================
class Box : public Mesh {
public:
	glm::vec3 bounds[2];
	Box(void);
	Box(glm::vec3  b0, glm::vec3  b1, glm::vec3 _colour);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 calNormal(glm::vec3 _p0);
	glm::vec3 getNormalPlane(glm::vec3 _p0);
	glm::vec3 getbBounds() { return *this->bounds; }
	glm::vec3 centroid();

	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
};
