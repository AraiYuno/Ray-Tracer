#pragma once
#include "Light.h"

class DirLight : public Light {
public:
	DirLight(void);
	DirLight(const glm::vec3 _p, const glm::vec3 _I);
	void lights();
};
