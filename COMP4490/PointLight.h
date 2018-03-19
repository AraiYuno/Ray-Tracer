#pragma once
#include "Light.h"

class PointLight : public Light {
public:
	PointLight(void);
	PointLight(const glm::vec3 _p, const glm::vec3 _i);
	void lights();
};