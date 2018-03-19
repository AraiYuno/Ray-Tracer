#include "Box.h"

// BOX INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Box::Box(void) {
	bounds[0] = glm::vec3(0);
	bounds[1] = glm::vec3(0);
	colour = options.backgroundColour;
}


Box::Box(glm::vec3  b0, glm::vec3  b1, glm::vec3 _colour) {
	bounds[0] = b0;
	bounds[1] = b1;
	colour = _colour;
}

bool Box::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	glm::vec3 invDir = 1.0f / _rayDirection;           // Inverse ray direction
	int sign[3];
	sign[0] = (invDir.x < 0);
	sign[1] = (invDir.y < 0);
	sign[2] = (invDir.z < 0);

	tmin = (bounds[sign[0]].x - _rayOrigin.x) * invDir.x;
	tmax = (bounds[1 - sign[0]].x - _rayOrigin.x) * invDir.x;
	tymin = (bounds[sign[1]].y - _rayOrigin.y) * invDir.y;
	tymax = (bounds[1 - sign[1]].y - _rayOrigin.y) * invDir.y;

	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[sign[2]].z - _rayOrigin.z) * invDir.z;
	tzmax = (bounds[1 - sign[2]].z - _rayOrigin.z) * invDir.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	*t = tmin;
	if (*t < 0) {
		*t = tmax;
		if (*t < 0) {
			return false;
		}
	}
	return true;
}

glm::vec3 Box::calNormal(glm::vec3 _p0) {
	N = getNormalPlane(_p0);
	return N;
}

glm::vec3 Box::getNormalPlane(glm::vec3 _p0) {
	glm::vec3 toReturn = glm::vec3(0, 0, 0);
	if (_p0.z + 0.0005f > bounds[1].z && _p0.z - 0.0005f < bounds[1].z) // point intersected with the front of the box.
		toReturn = glm::vec3(0, 0, 1);
	else if (_p0.z + 0.0005f > bounds[0].z && _p0.z - 0.0005f < bounds[0].z)
		toReturn = glm::vec3(0, 0, -1);
	else if (_p0.x + 0.0005f > bounds[1].x && _p0.x - 0.0005f < bounds[1].x)
		toReturn = glm::vec3(1, 0, 0);
	else if (_p0.x + 0.0005f > bounds[0].x && _p0.x - 0.0005f < bounds[0].x)
		toReturn = glm::vec3(-1, 0, 0);
	else if (_p0.y + 0.0005f > bounds[1].y && _p0.y - 0.0005f < bounds[1].y)
		toReturn = glm::vec3(0, 1, 0);
	else if (_p0.y + 0.0005f > bounds[0].y && _p0.y - 0.0005f < bounds[0].y)
		toReturn = glm::vec3(0, -1, 0);
	return toReturn;
}

glm::vec3 Box::centroid() {
	glm::vec3 b0 = this->bounds[0];
	glm::vec3 b1 = this->bounds[1];
	float x = (b1.x + b0.x) / 2;
	float y = (b1.y + b0.y) / 2;
	float z = (b1.z + b0.z) / 2;
	return glm::vec3(x, y, z);
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Box::getMaxX() { return glm::max(this->bounds[0].x, this->bounds[1].x); }
float Box::getMaxY() { return glm::max(this->bounds[0].y, this->bounds[1].y); }
float Box::getMaxZ() { return glm::max(this->bounds[0].z, this->bounds[1].z); }
float Box::getMinX() { return glm::min(this->bounds[0].x, this->bounds[1].x); }
float Box::getMinY() { return glm::min(this->bounds[0].y, this->bounds[1].y); }
float Box::getMinZ() { return glm::min(this->bounds[0].z, this->bounds[1].z); }
