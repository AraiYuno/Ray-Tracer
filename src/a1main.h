#include <iostream>
#include <glm/glm.hpp>
using namespace std;

//Implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);

// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);

//======================================================================
// Sphere Intersection
//    Code for sphere intersection
//======================================================================
void renderSI(void *window, int width, int height);
class Sphere {
public:
	glm::vec3 pos;
	glm::vec3 colour;
	float radius;

	Sphere(void);

	Sphere(glm::vec3 _pos, float _radius, glm::vec3 _colour);

	bool Intersection(float *t, glm::vec3 _rayOrigin, glm::vec3 _rayDirection);
};

