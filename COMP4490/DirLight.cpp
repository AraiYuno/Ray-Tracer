#include "DirLight.h"


DirLight::DirLight(void) { this->pos = glm::vec3(0), this->intensity = glm::vec3(1); }
void DirLight::lights() { }
DirLight::DirLight(const glm::vec3 _p, glm::vec3 _i) {
	this->pos = _p;
	this->intensity = _i;
}
