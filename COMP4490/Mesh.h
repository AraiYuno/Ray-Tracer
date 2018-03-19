#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>


enum SurfaceMaterial { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION, CHECKERBOARD };
//======================================================================
// Mesh 
//    Mesh class in which Sphere and Triangle inherits from it
//======================================================================
class Mesh {
public:
	int num;
	glm::vec3 pos, colour, N, diffuseColour, specularColour, reflectiveColour, transmissive, ambientColour;
	float Kd, Ks, specularExponent, ior, shininess;
	SurfaceMaterial surfaceMaterial;
	Mesh(void);
	Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N);

	virtual bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	virtual glm::vec3 centroid();
	virtual glm::vec3 calNormal(glm::vec3 _p0);
	virtual void setSurfaceProperties(float _shininess, float _ior, glm::vec3 _diffuseColour, glm::vec3 _specularColour, glm::vec3 _ambientColour,
		glm::vec3 _transmissive, glm::vec3 _reflectiveColour);

	virtual float getMinX();
	virtual float getMinY();
	virtual float getMinZ();
	virtual float getMaxX();
	virtual float getMaxY();
	virtual float getMaxZ();
};

#endif