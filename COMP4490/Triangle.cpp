#include "Triangle.h"


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
	float x, f, u1, v1;
	glm::vec3 h, s, q;

	glm::vec3 edge1 = b - a;
	glm::vec3 edge2 = c - a;

	h = glm::cross(_rayDirection, edge2);
	x = glm::dot(edge1, h);
	if (x > -EPSILON && x < EPSILON)
		return false;

	f = 1.0 / x;
	s = _rayOrigin - a;
	u = f * (glm::dot(s, h));
	if (u < 0.0 || u > 1.0)
		return false;

	q = glm::cross(s, edge1);
	v = f * (glm::dot(_rayDirection, q));
	if (v < 0.0 || u + v > 1.0)
		return false;
	w = 1.0f - u - v;
	// By now we can surely compute t* to check where the intersection point is on the line.
	float t0 = f * (glm::dot(edge2, q));
	if (t0 > EPSILON) {   // Ray intersection in this case
		*t = t0;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection
		return false;
}


//====================================================================================
// centroid
//    get coordinates of centroid of the triangle
//====================================================================================
glm::vec3 Triangle::centroid() {
	float x = (this->a.x + this->b.x + this->c.x) / 3.0f;
	float y = (this->a.y + this->b.y + this->c.y) / 3.0f;
	float z = (this->a.z + this->b.z + this->c.z) / 3.0f;
	return glm::vec3(x, y, z);
}


// Calculation for normal on 
float getNormalX(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return (A.y * B.z) - (A.z * B.y);
}
float getNormalY(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return (A.z * B.x) - (A.x * B.z);
}
float getNormalZ(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return (A.x * B.y) - (A.y * B.x);
}

// Calculating triangle's normal
glm::vec3 getNormal(Triangle *triangle) {
	float triNormX = getNormalX(triangle->a, triangle->b, triangle->c);
	float triNormY = getNormalY(triangle->a, triangle->b, triangle->c);
	float triNormZ = getNormalZ(triangle->a, triangle->b, triangle->c);
	return glm::vec3(triNormX, triNormY, triNormZ);
}

glm::vec3 Triangle::calNormal(glm::vec3 _p0) {
	float normalX = getNormalX(this->a, this->b, this->c);
	float normalY = getNormalY(this->a, this->b, this->c);
	float normalZ = getNormalZ(this->a, this->b, this->c);
	//getNormal(this);
	glm::vec3(1, 0, 0);

	glm::vec3 edge0 = glm::normalize(b - a);
	glm::vec3 edge1 = glm::normalize(c - b);
	return glm::normalize(glm::cross(edge0, edge1));
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Triangle::getMaxX() { return glm::max(glm::max(this->a.x, this->b.x), this->c.x); }
float Triangle::getMaxY() { return glm::max(glm::max(this->a.y, this->b.y), this->c.y); }
float Triangle::getMaxZ() { return glm::max(glm::max(this->a.z, this->b.z), this->c.z); }
float Triangle::getMinX() { return glm::min(glm::min(this->a.x, this->b.x), this->c.x); }
float Triangle::getMinY() { return glm::min(glm::min(this->a.y, this->b.y), this->c.y); }
float Triangle::getMinZ() { return glm::min(glm::min(this->a.z, this->b.z), this->c.z); }



