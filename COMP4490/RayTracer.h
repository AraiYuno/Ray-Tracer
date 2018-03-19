#include <list>
#include <glm/glm.hpp>
#include <ctime>
#include <iostream>
#include "Mesh.h"
#include "Light.h"
#pragma once
using namespace std;

struct Options {
	float fov = 90, bias = 1e-5f;
	int maxDepth = 5;
	glm::vec3 backgroundColour = glm::vec3(0.78, 0.67, 0.65);
	float constant_attenuation = 0, linear_attenuation = 0, quadratic_attenuation = 1;
};


void renderSI(void *window, int width, int height);
void fresnel(const glm::vec3 _I, const glm::vec3 _N, const float *ior, float *kr);
glm::vec3 refract(const glm::vec3 &_I, const glm::vec3 &_N, const float *ior);
glm::vec3 reflect(const glm::vec3 _I, const glm::vec3 _N);
bool traceRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, float *t, int &meshHitIndex, Mesh **hitMesh);
glm::vec3 castRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, int depth);
glm::vec3 setToOrigRGB(glm::vec3 _colour);
float clamp(const float _lo, const float _hi, const float _v);
