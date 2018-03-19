#include <glm/glm.hpp>
#pragma once
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
