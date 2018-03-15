#include <iostream>
#include <glm/glm.hpp>
#include <FL/vector3.h>
using namespace std;

//options.
struct Options {
	float fov = 90, bias = 0.0001;
	int maxDepth = 5;
};


//Implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);


// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);

// Different surface material types
enum SurfaceMaterial { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };



//======================================================================
// Light
//    stores the properties of a ray
//======================================================================
class Light {
public:
	glm::vec3 pos, intensity;
	Light(void);
	Light(const glm::vec3 _p, const glm::vec3 _i);
	virtual void lights() = 0;
};

class PointLight : public Light {
public:
	PointLight(void);
	PointLight(const glm::vec3 _p, const glm::vec3 _i);
	void lights();
};

class DirLight : public Light {
public:
	DirLight(void);
	DirLight(const glm::vec3 _p, const glm::vec3 _I);
	void lights();
};



glm::vec3 reflect(const glm::vec3 _I, const glm::vec3 _N);

//======================================================================
// Mesh 
//    Mesh class in which Sphere and Triangle inherits from it
//======================================================================
class Mesh {
public:
	glm::vec3 pos, colour, N, diffuseColour;
	float Kd, Ks, specularExponent, ior;
	SurfaceMaterial surfaceMaterial;
	Mesh(void);
	Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N);

	virtual bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	virtual glm::vec3 calNormal(int *shininess, glm::vec3 _p0, glm::vec3 *diffuse, glm::vec3 *specular);
};


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
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
};


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
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
};


//======================================================================
// Box Intersection
//    Code for Box Intersection
//======================================================================
class Box : public Mesh {
public:
	glm::vec3 bounds[2];
	Box(glm::vec3  b0, glm::vec3  b1, glm::vec3 _colour);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
	glm::vec3 getNormalPlane(glm::vec3 _p0);
};




//======================================================================
// Shadow Attenuation
//    Code for Shadow Attenuation
//======================================================================
class ShadowAtt {
public:
	glm::vec3 pos, size;
	ShadowAtt(void);
	ShadowAtt(glm::vec3 _pos, glm::vec3 _size);
	~ShadowAtt();
};



// to render the scene.
void renderSI(void *window, int width, int height);
void createMeshes(Mesh *meshes[]);
void createLights(Light *Lights[]);
void fresnel(const glm::vec3 &_I, const glm::vec3 &_N, const float *ior, float *kr);
bool traceRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, float *t, int &meshHitIndex, Mesh **hitMesh);
glm::vec3 castRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection , int depth);
glm::vec3 setToOrigRGB(glm::vec3 _colour);
glm::vec3 refract(const glm::vec3 &_I, const glm::vec3 &_N, const float *ior);
float clamp(const float _lo, const float _hi, const float _v);