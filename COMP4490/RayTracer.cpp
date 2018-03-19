#include "RayTracer.h"




//=================================================================================
//renderSI
//  renders sphere intersection using raytracing.
//
//=================================================================================
void renderSI(void * window, int width, int height, list<Mesh*> *meshList, list<Light*> *lightList )
{
	meshList = new list<Mesh*>();
	lightList = new list<Light*>();
	int selector = uiStarter->sceneSelector();
	createMeshes(globalMeshList, selector);
	createLights(globalLightList, selector);
	bool bvhButton = bvhSwitch();
	clock_t begin = clock();
	glm::vec3 **image = new glm::vec3*[width];
	for (int i = 0; i < width; i++)
		image[i] = new glm::vec3[height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float pixRemapX = (2 * ((x + 0.5f) / (float)width) - 1)*(float)(width / height);
			float pixRemapY = 1 - 2 * ((y + 0.5f) / (float)height);
			float pixCamX = pixRemapX * glm::tan(glm::radians(options.fov) / 2);
			float pixCamY = pixRemapY * glm::tan(glm::radians(options.fov) / 2);
			glm::vec3 PcamSpace = glm::vec3(-pixRemapX, pixRemapY, 1);
			glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
			glm::vec3 rayDirection = glm::normalize(rayOrigin - PcamSpace);
			glm::vec3 pixColour = options.backgroundColour;
			if (bvhButton) {
				bool isHit = false;
				bvh->findIntersection(bvh->root, rayOrigin, rayDirection, &isHit);
				if (isHit) {
					pixColour = castRay(rayOrigin, rayDirection, 0);
				}
			}
			else
				pixColour = castRay(rayOrigin, rayDirection, 0);
			pixColour = setToOrigRGB(pixColour);
			set(window, x, y, pixColour.x, pixColour.y, pixColour.z);
		}
	}
	clock_t end = clock();
	double elapsed_time = double(end - begin);
	cout << "Time Elapsed: " << elapsed_time << " ms" << endl;
}

glm::vec3 setToOrigRGB(glm::vec3 _colour) {
	glm::vec3 toReturn = _colour * 255.0f; // switch from (0, 1) scale to (0, 255)
										   // set the boundaries
	if (toReturn.x >= 255.0f) {
		toReturn.x = 255.0f;
	}
	if (toReturn.x <= 0.0f)
		toReturn.x = 0.0f;
	if (toReturn.y >= 255.0f)
		toReturn.y = 255.0f;
	if (toReturn.y <= 0.0f)
		toReturn.y = 0.0f;
	if (toReturn.z >= 255.0f)
		toReturn.z = 255.0f;
	if (toReturn.z <= 0.0f)
		toReturn.z = 0.0f;
	return toReturn;
}



glm::vec3 castRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, int depth) {
	if (depth > maxDepth) // if it's higher than the maxDepth, we just return a white colour
		return options.backgroundColour;
	glm::vec3 phongShade = glm::vec3(0);
	glm::vec3 ambientShade = glm::vec3(0);
	glm::vec3 hitColour = options.backgroundColour;
	float tNear = 0.0f;
	int hitMeshIndex = 0;
	Mesh *hitMesh = nullptr;
	// if there is an intersetion with a mesh in the scene
	// Here I need the BVH function to quickly determine if i need to traceRay or not
	if (traceRay(_rayOrigin, _rayDirection, &tNear, hitMeshIndex, &hitMesh)) {
		glm::vec3 p0 = _rayOrigin + _rayDirection * tNear;
		glm::vec3 N = hitMesh->calNormal(p0);
		glm::vec3 tempP0 = p0;
		switch (hitMesh->surfaceMaterial) {
		case DIFFUSE_AND_GLOSSY:
		{
			glm::vec3 lightAmount = glm::vec3(0);
			// lights sizes

			Light *tempLight = nullptr;
			int num = 0;
			for (list<Light*>::iterator it = globalLightList->begin(); it != globalLightList->end(); ++it) {
				tempLight = &**it;
				glm::vec3 lightDirection = glm::vec3(0);
				if (dynamic_cast<DirLight*>(tempLight))
					lightDirection = glm::normalize(tempLight->pos);
				else
					lightDirection = glm::normalize(tempLight->pos - p0);

				// AMBIENT
				glm::vec3 ambient = hitMesh->colour * hitMesh->ambientColour;
				// DIFFUSE 
				glm::vec3 diffuse = hitMesh->diffuseColour * tempLight->intensity * glm::max(0.0f, glm::dot(lightDirection, N));
				// SPECULAR
				glm::vec3 reflection = glm::normalize(2 * (glm::dot(lightDirection, N)) * N - lightDirection);
				float maxCalc = glm::max(0.0f, glm::dot(reflection, glm::normalize(_rayOrigin - p0)));
				glm::vec3 specular = hitMesh->specularColour * tempLight->intensity * glm::pow(maxCalc, hitMesh->shininess);
				// DISTANCE ANNUTATION
				float distance = glm::distance(tempLight->pos, p0);
				float attenuation = 1.0f / (options.constant_attenuation + options.linear_attenuation*distance + options.quadratic_attenuation*distance*distance);
				;

				float tNearShadow = INFINITY;
				Mesh * shadowHitMesh = nullptr;
				bool inShadow = traceRay(p0 + (1e-4f * N), lightDirection, &tNearShadow, hitMeshIndex, &shadowHitMesh);
				if (inShadow) {
					ambientShade += hitMesh->colour * ambient;
				}
				else {
					phongShade += (specular + diffuse)*attenuation;
				}
			}
			glm::vec3 Nhit;
			glm::vec2 tex;
			if (dynamic_cast<Sphere*>(hitMesh) && hitMesh->surfaceMaterial == CHECKERBOARD) {
				Sphere *sphere = dynamic_cast<Sphere*>(hitMesh);
				sphere->getSurfaceData(p0, &Nhit, &tex);
				float scale = 4;
				float pattern = (fmodf(tex.x * scale, 1) > 0.5) ^ (fmodf(tex.y * scale, 1) > 0.5);
				hitColour = glm::max(glm::vec3(0.0f), (glm::dot(Nhit, -_rayDirection) * glm::mix((ambientShade + phongShade), (ambientShade + phongShade) * 0.5f, pattern)));
			}
			else
				hitColour = ambientShade + phongShade;
			break;
		}
		case REFLECTION:
		{
			float kr;
			fresnel(_rayDirection, N, &hitMesh->ior, &kr);
			glm::vec3 reflectionDirection = reflect(_rayDirection, N);
			glm::vec3 reflectionRayOrig = (glm::dot(reflectionDirection, N) < 0) ?
				p0 + N * options.bias :
				p0 - N * options.bias;
			hitColour = hitMesh->colour * (kr*13.0f) * castRay(reflectionRayOrig, reflectionDirection, depth + 1);
			break;
		}
		case REFLECTION_AND_REFRACTION:
		{
			glm::vec3 refractionDirection = glm::normalize(refract(_rayDirection, N, hitMesh->ior));
			glm::vec3 refractionRayOrig = (glm::dot(refractionDirection, N) < 0) ?
				p0 - N * options.bias :
				p0 + N * options.bias;
			float kr;
			fresnel(_rayDirection, N, &hitMesh->ior, &kr);
			hitColour = castRay(p0*(N), refractionDirection, depth + 1);
			break;
		}
		}

	}
	return hitColour;
}
//=======================================================================================
// trace
//   returns true if the ray intersects with an object. This function also assign t and
//   meshHitIndex value.
//=======================================================================================
bool traceRay(const glm::vec3 &_rayOrigin, const glm::vec3 &_rayDirection, float *_t, int &_meshHitIndex, Mesh **_hitMesh) {
	*_hitMesh = nullptr;
	float minT = INFINITY;
	float t0 = 0.0f;

	Mesh *tempMesh = nullptr;
	int index = 0;
	for (list<Mesh*>::iterator it = globalMeshList->begin(); it != globalMeshList->end(); ++it) {
		tempMesh = &**it;
		bool hit = tempMesh->Intersection(_rayOrigin, _rayDirection, &t0);
		if (hit && t0 < minT) {
			*_hitMesh = tempMesh;
			minT = t0;
			*_t = t0;
			_meshHitIndex = index;
		}
		index++;
	}
	return (*_hitMesh != nullptr);
}

float clamp(const float _lo, const float _hi, const float _v) {
	return glm::max(_lo, glm::min(_hi, _v));
}


void fresnel(const glm::vec3 _I, const glm::vec3 _N, const float *ior, float *kr)
{
	float cosi = clamp(-1, 1, glm::dot(_I, _N));
	float etai = 1, etat = (*ior);
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(glm::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		*kr = 1;
	}
	else {
		float cost = sqrtf(glm::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		*kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}


glm::vec3 reflect(const glm::vec3 _I, const glm::vec3 _N) {
	return _I - 2 * glm::dot(_I, _N) *_N;
}


glm::vec3 refract(const glm::vec3 &_I, const glm::vec3 &_N, const float *ior)
{
	/*float cosi = -glm::dot(_N, _I);
	float etat = *ior;
	float sinT2 = etat * etat * (1.0f - cosi * cosi);
	float cosT = sqrt(1.0f - sinT2);
	return _I * etat + _N * (_N * cosi - cosT);
	*/
	float cosi = clamp(-1, 1, glm::dot(_I, _N));
	float etai = 1, etat = *ior;
	glm::vec3 n = _N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -_N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? glm::vec3(0) : eta * _I + (eta * cosi - sqrtf(k)) * n;
}

