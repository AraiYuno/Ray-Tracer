#include "PointLight.h"

PointLight::PointLight(void) { this->pos = glm::vec3(0), this->intensity = glm::vec3(1); }
void PointLight::lights() { }
PointLight::PointLight(const glm::vec3 _p, glm::vec3 _i) {
	this->pos = _p;
	this->intensity = _i;
}