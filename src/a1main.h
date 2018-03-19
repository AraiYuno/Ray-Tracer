#include <iostream>
#include <list>
#include <glm/glm.hpp>
#include <FL/vector3.h>



//Implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);


// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);

















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

