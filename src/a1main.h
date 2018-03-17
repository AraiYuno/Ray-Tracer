#include <iostream>
#include <list>
#include <glm/glm.hpp>
#include <FL/vector3.h>
using namespace std;


struct Options {
	float fov = 90, bias = 1e-5f;
	int maxDepth = 5;
	glm::vec3 backgroundColour = glm::vec3(0.78, 0.67, 0.65);
};


//Implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);


// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);

// Different surfac material typestex
enum SurfaceMaterial { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION, CHECKERBOARD };


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
	int num;
	glm::vec3 pos, colour, N, diffuseColour, specularColour, reflectiveColour, transmissive;
	float Kd, Ks, specularExponent, ior;
	SurfaceMaterial surfaceMaterial;
	Mesh(void);
	Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N);

	virtual bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	virtual glm::vec3 centroid();
	virtual glm::vec3 calNormal(int *shininess, glm::vec3 _p0, glm::vec3 *diffuse, glm::vec3 *specular);

	virtual float getMinX();
	virtual float getMinY();
	virtual float getMinZ();
	virtual float getMaxX();
	virtual float getMaxY();
	virtual float getMaxZ();
};


//======================================================================
// Sphere Intersection
//    Code for sphere intersection
//======================================================================
class Sphere : public Mesh {
public:
	float radius;
	Sphere(void);
	Sphere(glm::vec3 _pos, glm::vec3 _colour, float _radius, glm::vec3 _diffuseColour, 
			glm::vec3 _specularColour, glm::vec3 _reflectiveColour, glm::vec3 _transmissive);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 centroid();
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
	void getSurfaceData(glm::vec3 p0, glm::vec3 *_N, glm::vec2 *tex);

	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
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
	glm::vec3 centroid();
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);

	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
};


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
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
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

// Node to be used in BVH. I am using binary tree structure for my BVH
class Node {
public:
	Node(void);
	// Mutators
	void addMesh(Mesh* mesh);
	Box *bbox;
	Node *left, *right;
	list<Mesh*> *meshList;

	void printMeshes();
};


class BVH {


public:
	Node * root;
	// This function takes in all the objects and creates a BVH tree.
	BVH(void);
	void initiateBVH();
	void setBBox(Node *curr);
	void listSplit(Node *curr, Node *tempLeft, Node *tempRight);
	void traverseTest(Node *curr);
	bool buildBVH(Node *curr);
	bool findIntersection(Node *curr, glm::vec3 _rayOrigin, glm::vec3 _rayDirection, bool *isHit);
};


class Checkerboard {
private:
	glm::vec3 colour1;
	glm::vec3 colour2;
	float scale;
	float shininess;
	float reflectivity;

public:
	Checkerboard(void);
	Checkerboard(glm::vec3 _colour1, glm::vec3 _colour2, float _scale, float _shininess, float _reflectivity);

	virtual glm::vec3 getColour(glm::vec3 point);
	virtual float getShininess();
	virtual float getReflectivity();
};



// to render the scene.
bool bvhSwitch();
void renderSI(void *window, int width, int height);
void createMeshes(Mesh *meshes[]);
void createLights(Light *Lights[]);
void fresnel(const glm::vec3 _I, const glm::vec3 _N, const float *ior, float *kr);
glm::vec3 refract(const glm::vec3 &_I, const glm::vec3 &_N, const float *ior);
glm::vec3 reflect(const glm::vec3 _I, const glm::vec3 _N);
bool traceRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, float *t, int &meshHitIndex, Mesh **hitMesh);
glm::vec3 castRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, int depth);
glm::vec3 setToOrigRGB(glm::vec3 _colour);
float clamp(const float _lo, const float _hi, const float _v);

