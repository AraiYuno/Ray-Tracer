#include <iostream>
#include <glm/glm.hpp>
using namespace std;

//Implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);

// to render the scene.
void renderSI(void *window, int width, int height);

// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);


//======================================================================
// Mesh 
//    Mesh class in which Sphere and Triangle inherits from it
//======================================================================
class Mesh {
public:
	glm::vec3 pos, colour, N;
	Mesh(void);
	Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N);
	virtual bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);

	glm::vec3 calNormal(int *shininess, glm::vec3 _p0, glm::vec3 *diffuse, glm::vec3 *specular);
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
	Triangle(void);
	Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _colour);
	bool Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t);
	glm::vec3 calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular);
};

