#include "Mesh.h"

#include <glm/glm.hpp>

// MESH DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Mesh::Mesh(void) {
	pos = glm::vec3(0, 0, 0);
	colour = glm::vec3(0, 0, 0);
	N = glm::vec3(0, 0, 0);  // Normal
	surfaceMaterial = DIFFUSE_AND_GLOSSY;
	Kd = 0.8;
	Ks = 0.2;
	ior = 1.0;
	diffuseColour = glm::vec3(0.8);
	specularExponent = 25;
}

Mesh::Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N) {
	pos = _position;
	colour = _colour;
	N = _N;
	surfaceMaterial = DIFFUSE_AND_GLOSSY;
	Kd = 0.8;
	Ks = 0.2;
	ior = 1.0;
	diffuseColour = glm::vec3(0.8);
	specularExponent = 25;
}


bool Mesh::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	return false;
}

glm::vec3 Mesh::centroid() {
	return this->pos;
}

glm::vec3 Mesh::calNormal(glm::vec3 _p0) {
	return N;
}

void Mesh::setSurfaceProperties(float _shininess, float _ior, glm::vec3 _diffuseColour, glm::vec3 _specularColour, glm::vec3 _ambientColour,
	glm::vec3 _transmissive, glm::vec3 _reflectiveColour) {
	this->shininess = _shininess;
	this->ior = _ior;
	this->diffuseColour = _diffuseColour;
	this->specularColour = _specularColour;
	this->ambientColour = _ambientColour;
	this->transmissive = _transmissive;
	this->reflectiveColour = _reflectiveColour;
}

float Mesh::getMaxX() { return this->pos.x; }
float Mesh::getMaxY() { return this->pos.y; }
float Mesh::getMaxZ() { return this->pos.z; }
float Mesh::getMinX() { return this->pos.x; }
float Mesh::getMinY() { return this->pos.y; }
float Mesh::getMinZ() { return this->pos.z; }