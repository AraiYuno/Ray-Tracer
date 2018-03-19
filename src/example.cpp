#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include "a1main.h"



using namespace std;
struct Options;

//GLOBAL VARIABLE
int countTraceRay = 0;
BVH* bvh;
Options options;	
int maxDepth = 5;
list<Mesh*> *globalMeshList;
list<Light*> *globalLightList;
Light* lights[2];
bool hardShadow = true;

void trace(char *input_file, void *window, int width, int height) {
	renderSI(window, width, height);             // Rendering Sphere Intersection
	cout << "Total Number of TraceRay calling: " << countTraceRay << endl;
	cout << "End of the Program" << endl;
}

void pick(void *window, int x, int y) {
	unsigned char r, g, b;
	if (get(window, x, y, r, g, b)) {
		printf("pick @ (%d, %d): colour [%d %d %d]\n", x, y, r, g, b);
		set(window, x, y, (13+r)%256, (25+g)%256, (41+b)%256);
		redraw(window);
	}
}

bool bvhSwitch() {
	int bvhButton = 0;
	cout << "Please type 1 if you want to turn on BVH. Type 0 otherwise." << endl;
	cin >> bvhButton;
	if (bvhButton == 1) {
		bvh = new BVH(); // Here we have built our acceleration structure.
	}
	return bvhButton;
}

int sceneSelector() {
	int selector = 0;
	cout << "Please specify which scene you would like to render. Press a number. ex) 1 for boxes.ray." << endl;
	cout << "1. boxes.ray" << endl;
	cout << "2. sphere.ray" << endl;
	cout << "3. easy1.ray" << endl;

	cin >> selector;
	return selector;
}

//=================================================================================
//renderSI
//  renders sphere intersection using raytracing.
//
//=================================================================================
void renderSI(void * window, int width, int height )
{
	
	globalMeshList = new list<Mesh*>();
	globalLightList = new list<Light*>();
	int selector = sceneSelector();
	createMeshes(globalMeshList, selector);
	createLights(globalLightList, selector);
	bool bvhButton = bvhSwitch();
	clock_t begin = clock();
	glm::vec3 **image = new glm::vec3*[width];
	for (int i = 0; i < width; i++) 
		image[i] = new glm::vec3[height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float pixRemapX = (2 * ((x + 0.5f) / (float)width) - 1)*(float)(width/height) ;
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


void createMeshes(list<Mesh*> *meshList, int selector) {
	// SPHERE
	
	//boxes.ray
	if (selector == 1) {
		cout << "HERE " << endl;
		options.fov = 45.0f;
		Box *box1 = new Box(glm::vec3(0.25f, 0.25f, -3.8), glm::vec3(0.75, 0.75, -2.7), glm::vec3(0, 0.9, 0));
		box1->setSurfaceProperties(76.8f, 1.0f, glm::vec3(box1->colour), glm::vec3(0.0, 0.8, 0), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.0f, 0.9f, 0.0f));
		meshList->push_back(box1);

		Box *box2 = new Box(glm::vec3(0.50f, -1.25f, -4.0), glm::vec3(1.0f, -0.75f, -3.0f), glm::vec3(0.7, 0, 1));
		box2->setSurfaceProperties(76.8f, 1.5f, glm::vec3(box2->colour), glm::vec3(0.9, 0.4, 0), glm::vec3(0.2f), glm::vec3(0.9, 0.4, 0), glm::vec3(0.0f));
		box2->surfaceMaterial = REFLECTION_AND_REFRACTION;
		meshList->push_back(box2);

		Box *box3 = new Box(glm::vec3(-1.0f, -1.05f, -4.5), glm::vec3(0.2f, 0.55f, -3.5f), glm::vec3(0, 0.7, 1));
		box3->setSurfaceProperties(76.8f, 1.5f, glm::vec3(box3->colour), glm::vec3(0, 0.9, 0.4), glm::vec3(0.2f),glm::vec3(0.0f), glm::vec3(0, 0.9, 0.4));
		box3->surfaceMaterial = REFLECTION;
		meshList->push_back(box3);
	}
	// sphere.ray 
	else if (selector == 2) {
		Sphere *sphere1 = new Sphere(glm::vec3(1.85604, -0.443595, -5.13845), glm::vec3(0.8, 0.2, 0.5), 0.310878f);
		sphere1->setSurfaceProperties(25.6f, 1.0f, sphere1->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.0f, 0.9f, 0.0f));
		meshList->push_back(sphere1);

		Sphere *sphere2 = new Sphere(glm::vec3(-0.800517, 1.81094, -5.62886), glm::vec3(0.5, 0.75, 0.2), 0.312566);
		sphere2->setSurfaceProperties(25.6f, 1.3f, sphere2->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.0f, 0.9f, 0.0f));
		meshList->push_back(sphere2);

		Sphere *sphere3 = new Sphere(glm::vec3(1.15027, -0.522035, -5.43002), glm::vec3(0.34, 0.07, 0.56), 0.670496);
		sphere3->setSurfaceProperties(122.074f, 1.3f, sphere3->colour, glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.4, 0.4, 0.4));
		sphere3->surfaceMaterial = REFLECTION;
		meshList->push_back(sphere3);

		Sphere *sphere4 = new Sphere(glm::vec3(-0.560763, 1.08614, -5.70474), glm::vec3(0.56, 0.24, 0.12), 0.636615);
		sphere4->setSurfaceProperties(120.889f, 1.3f, sphere4->colour, glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.4, 0.4, 0.4));
		sphere4->surfaceMaterial = REFLECTION;
		meshList->push_back(sphere4);

		Sphere *sphere5 = new Sphere(glm::vec3(-0.24724, -0.229819, -6.12079), glm::vec3(0.6, 0.6, 0.6), 1.18902);
		sphere5->setSurfaceProperties(118.519, 1.0f, sphere5->colour, glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.4, 0.4, 0.4));
		sphere5->surfaceMaterial = DIFFUSE_AND_GLOSSY;
		meshList->push_back(sphere5);
	}
	else if (selector == 3) {
		Sphere *sphere1 = new Sphere(glm::vec3(-0.42001, 1.59499, -12.2546), glm::vec3(0.56, 0.35, 0.14), 1.33453f);
		sphere1->setSurfaceProperties(25.6f, 1.0f, sphere1->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.0f, 0.9f, 0.0f));
		meshList->push_back(sphere1);

		Triangle *triangle1 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(0.0379083, -3.07572, -7.9795), glm::vec3(6.58523, 0.753526, -13.3846), glm::vec3(0.56, 0.5, 0.43));
		triangle1->setSurfaceProperties(25.6f, 1.0f, triangle1->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle1);

		Triangle *triangle2 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(6.58523, 0.753526, -13.3846), glm::vec3(6.56273, 1.04789, -13.2033), glm::vec3(0.56, 0.5, 0.43));
		triangle2->setSurfaceProperties(25.6f, 1.0f, triangle2->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle2);

		Triangle *triangle3 = new Triangle(glm::vec3(-6.77845, 0.385666, -13.7841), glm::vec3(-0.231127, 4.21491, -19.1892), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(0.56, 0.5, 0.43));
		triangle3->setSurfaceProperties(25.6f, 1.0f, triangle3->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle3);

		Triangle *triangle4 = new Triangle(glm::vec3(-6.77845, 0.385666, -13.7841), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(-6.75595, 0.0913011, -13.9653), glm::vec3(0.56, 0.5, 0.43));
		triangle4->setSurfaceProperties(25.6f, 1.0f, triangle4->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle4);

		Triangle *triangle5 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(-6.77845, 0.385666, -13.7841), glm::vec3(-6.75595, 0.0913011, -13.9653), glm::vec3(0.56, 0.5, 0.43));
		triangle5->setSurfaceProperties(25.6f, 1.0f, triangle5->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle5);

		Triangle *triangle6 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(-6.75595, 0.0913011, -13.9653), glm::vec3(0.0379083, -3.07572, -7.9795), glm::vec3(0.56, 0.5, 0.43));
		triangle6->setSurfaceProperties(25.6f, 1.0f, triangle6->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle6);

		Triangle *triangle7 = new Triangle(glm::vec3(6.56273, 1.04789, -13.2033), glm::vec3(6.58523, 0.753526, -13.3846), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(0.56, 0.5, 0.43));
		triangle7->setSurfaceProperties(25.6f, 1.0f, triangle7->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle7);

		Triangle *triangle8 = new Triangle(glm::vec3(6.56273, 1.04789, -13.2033), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(-0.231127, 4.21491, -19.1892), glm::vec3(0.56, 0.5, 0.43));
		triangle8->setSurfaceProperties(25.6f, 1.0f, triangle8->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle8);

		Triangle *triangle9 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(6.56273, 1.04789, -13.2033), glm::vec3(-0.231127, 4.21491, -19.1892), glm::vec3(0.56, 0.5, 0.43));
		triangle9->setSurfaceProperties(25.6f, 1.0f, triangle9->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle9);

		Triangle *triangle10 = new Triangle(glm::vec3(0.0154059, -2.78136, -7.79821), glm::vec3(-0.231127, 4.21491, -19.1892), glm::vec3(-6.77845, 0.385666, -13.7841), glm::vec3(0.56, 0.5, 0.43));
		triangle10->setSurfaceProperties(25.6f, 1.0f, triangle10->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle10);

		Triangle *triangle11 = new Triangle(glm::vec3(0.0379083, -3.07572, -7.9795), glm::vec3(-6.75595, 0.0913011, -13.9653), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(0.56, 0.5, 0.43));
		triangle11->setSurfaceProperties(25.6f, 1.0f, triangle11->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle11);

		Triangle *triangle12 = new Triangle(glm::vec3(0.0379083, -3.07572, -7.9795), glm::vec3(-0.208624, 3.92055, -19.3705), glm::vec3(6.58523, 0.753526, -13.3846), glm::vec3(0.56, 0.5, 0.43));
		triangle12->setSurfaceProperties(25.6f, 1.0f, triangle12->colour, glm::vec3(0.8f), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(triangle12);
	}
	

	//meshList->push_back(new Sphere(glm::vec3(3, 5, -15), glm::vec3(0.1f, 0.1f, 0.1f), 2, glm::vec3(0.7f, 0.0f, 1.0f), glm::vec3(0.9f, 0.4f, 0.0f),
	//	glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.9, 0.4, 0))); // dark Floor
	//meshList->push_back(new Sphere(glm::vec3(9, -3, -15), glm::vec3(1.0f, 1.0f, 0), 3.1f, glm::vec3(0.7f, 0.0f, 1.0f), glm::vec3(0.9f, 0.4f, 0.0f),
	//	glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.9, 0.4, 0))); // YEE
	//meshList->push_back(new Sphere(glm::vec3(5, 0, -15), glm::vec3(1.0f, 1.0f, 1.0f), 1.5, glm::vec3(0.7f, 0.0f, 1.0f), glm::vec3(0.9f, 0.4f, 0.0f),
	//	glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.9, 0.4, 0))); // WHITE SPHERE
	//meshList->push_back(new Sphere(glm::vec3(-5, 0, -15), glm::vec3(1.0f, 1.0f, 0), 2, glm::vec3(0.7f, 0.0f, 1.0f), glm::vec3(0.9f, 0.4f, 0.0f),
	//	glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.9, 0.4, 0)));  // YELLOW SHPERE
	//meshes[2]->surfaceMaterial = REFLECTION_AND_REFRACTION;
	//meshes[2]->ior = 1.3f;
	//																			  // TRIANGULAR MESH
	//meshList->push_back(new Triangle(glm::vec3(-2, 6, -17), glm::vec3(2, 6, -15), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f))); // EMERALD TRIANGLE // u -> CAP
	//meshList->push_back(new Triangle(glm::vec3(2, 6, -15), glm::vec3(-2, 6, -13), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f))); // v -> ABP
	//meshList->push_back(new Triangle(glm::vec3(-2, 6, -13), glm::vec3(-2, 6, -17), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f))); // w -> BCP
	//meshList->push_back(new Triangle(glm::vec3(2, 6, -15), glm::vec3(-2, 6, -13), glm::vec3(-2, 6, -17), glm::vec3(0.08f, 0.33f, 0.08f))); // ABC -> base

	//																															   // BOXES
	//meshList->push_back(new Box(glm::vec3(-5, 3, -13), glm::vec3(-3, 5, -11), glm::vec3(0.20f, 0.20f, 1.0f)));
	//meshList->push_back(new Box(glm::vec3(-7, -5, -15), glm::vec3(-3, -1, -11), glm::vec3(1.0f, 0.078f, 0.58f)));

	//Sphere *mesh11 = new Sphere(glm::vec3(8, 3, -15), glm::vec3(0.1f, 0.1f, 1.0f), 2, glm::vec3(0.7f, 0.0f, 1.0f), glm::vec3(0.9f, 0.4f, 0.0f),
	//	glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.9, 0.4, 0)); // blue sphere
	//mesh11->surfaceMaterial = CHECKERBOARD;
	//meshList->push_back(mesh11);

	// set the idenfitier numbers
	Mesh *tempMesh = nullptr;
	int num = 0;
	for (list<Mesh*>::iterator it = meshList->begin(); it != meshList->end(); ++it) {
		tempMesh = &**it;
		tempMesh->num = num;
		num++;
	}
}

void createLights(list<Light*> *lightList, int selector) {
	if (selector == 1) {
		lightList->push_back(new PointLight(glm::vec3(0, 20, -1), glm::vec3(0.5, 0.5, 0.5)));
		lightList->push_back(new DirLight(glm::vec3(0, -1, 4), glm::vec3(1, 1, 1)));
		lightList->push_back(new DirLight(glm::vec3(0, -1, 4), glm::vec3(0.2, 0.2, 0.2)));
	}
	else if (selector == 2) {
		options.constant_attenuation = 0;
		options.linear_attenuation = 0;
		options.quadratic_attenuation = 1;
		lightList->push_back(new PointLight(glm::vec3(1.44564, -0.485372, 0.808535), glm::vec3(1, 1, 1)));
		lightList->push_back(new DirLight(glm::vec3(-0.710523, -0.810018, -7.16584), glm::vec3(1, 1, 1)));
	}
	else if (selector == 3) {
		options.constant_attenuation = 0.25f;
		options.linear_attenuation = 0.00337241f;
		options.quadratic_attenuation = 4.5492e-05f;
		lightList->push_back(new PointLight(glm::vec3(4.29969, 0.859405, -7.52207), glm::vec3(1, 1, 1)));
		lightList->push_back(new PointLight(glm::vec3(-5.47326, 2.34592, -8.07537), glm::vec3(1, 1, 1)));
		lightList->push_back(new DirLight(glm::vec3(3.75341, 1.84043, -11.6705), glm::vec3(1, 1, 1)));
	}
	/*lightList->push_back(new PointLight(glm::vec3(15, 20, -11), glm::vec3(1, 1, 1)));
	lightList->push_back(new DirLight(glm::vec3(-30, 30, -30), glm::vec3(1, 1, 1)));*/
}

float mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
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
	countTraceRay++;
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

float clamp(const float _lo, const float _hi, const float _v){
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



///================================================================================
///================================================================================
/// HEADER FILE IMPLEMENTATION
///================================================================================
///================================================================================

// LIGHTS DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
Light::Light(void) { this->pos = glm::vec3(0), this->intensity = glm::vec3(1); }
void Light::lights() { }


PointLight::PointLight(void) { this->pos = glm::vec3(0), this->intensity = glm::vec3(1); }
void PointLight::lights() { }
PointLight::PointLight(const glm::vec3 _p, glm::vec3 _i) {
	this->pos = _p;
	this->intensity = _i;
}

DirLight::DirLight(void) { this->pos = glm::vec3(0), this->intensity = glm::vec3(1); }
void DirLight::lights() { }
DirLight::DirLight(const glm::vec3 _p, glm::vec3 _i) {
	this->pos = _p;
	this->intensity = _i;
}

// MESH DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Mesh::Mesh(void) {
	pos = glm::vec3(0, 0, 0);
	colour = glm::vec3(0, 0, 0);
	N = glm::vec3(0, 0, 0);  // Normal
	surfaceMaterial = DIFFUSE_AND_GLOSSY;
	Kd = 0.8;
	Ks = 0.2;
	ior = 1.0;
	diffuseColour = glm::vec3(0.8);
	specularExponent = 25;
}

Mesh::Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N) {
	pos = _position;
	colour = _colour;
	N = _N;
	surfaceMaterial = DIFFUSE_AND_GLOSSY;
	Kd = 0.8;
	Ks = 0.2;
	ior = 1.0;
	diffuseColour = glm::vec3(0.8);
	specularExponent = 25;
}


bool Mesh::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	return false;
}

glm::vec3 Mesh::centroid() {
	return this->pos;
}

glm::vec3 Mesh::calNormal(glm::vec3 _p0) {
	return N;
}

void Mesh::setSurfaceProperties(float _shininess, float _ior, glm::vec3 _diffuseColour, glm::vec3 _specularColour, glm::vec3 _ambientColour,
	glm::vec3 _transmissive, glm::vec3 _reflectiveColour) {
	this->shininess = _shininess;
	this->ior = _ior;
	this->diffuseColour = _diffuseColour;
	this->specularColour = _specularColour;
	this->ambientColour = _ambientColour;
	this->transmissive = _transmissive;
	this->reflectiveColour = _reflectiveColour;
}

float Mesh::getMaxX() { return this->pos.x; }
float Mesh::getMaxY() { return this->pos.y; }
float Mesh::getMaxZ() { return this->pos.z; }
float Mesh::getMinX() { return this->pos.x; }
float Mesh::getMinY() { return this->pos.y; }
float Mesh::getMinZ() { return this->pos.z; }




// SPHERE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Sphere::Sphere(void) {
	pos = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _pos, glm::vec3 _colour, float _radius) {
	pos = _pos;
	colour = _colour;
	radius = _radius;
}

//======================================================================================
// Intersection -> SPHPERE
//    returns true if the ray intersects with with a sphere
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
//    The link has the algorithm that I used.
//======================================================================================
bool Sphere::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	glm::vec3 L = pos - _rayOrigin;    // L = C - O
	float tca = glm::dot(L, _rayDirection);  // Tca = L scalar product D
	if (tca < 0) {		// if Tca < 0, the point is located behind the ray origin.
		return false;
	}
	else {
		float d = glm::sqrt((glm::dot(L, L)) - (tca * tca)); // d = L^2 - Tca^2.
		if (d > radius) { // if d > radius, the ray misses the sphere, and there is no intersection
			return false;
		} // if d <= radius, then the ray intersects with the sphere.
		else {
			float thc = glm::sqrt((radius*radius) - ((glm::dot(L, L)) - (tca * tca)) ); // Thc = sqrt( radius^2 -  d^2 )using pythagorean algorithm
			float t0 = tca - thc;
			*t = t0;  // t0 = Tca - Thc -> The point where the ray intersects with the sphere
			return true;
		}
	}
}


glm::vec3 Sphere::centroid() {
	return this->pos;
}

glm::vec3 Sphere::calNormal(glm::vec3 _p0) {
	N = glm::normalize(_p0 - pos);
	return N;
}

void Sphere::getSurfaceData(glm::vec3 p0, glm::vec3 *_N, glm::vec2 *tex) {
	*_N = p0 - this->pos;
	*_N = glm::normalize(*_N);
	tex->x = (1 + atan2(_N->z, _N->x) / (glm::atan(1) * 4)) * 0.5;
	tex->y = acosf(_N->y) / (glm::atan(1));
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Sphere::getMaxX() { return this->pos.x + this->radius; }
float Sphere::getMaxY() { return this->pos.y + this->radius; }
float Sphere::getMaxZ() { return this->pos.z + this->radius; }
float Sphere::getMinX() { return this->pos.x - this->radius; }
float Sphere::getMinY() { return this->pos.y - this->radius; }
float Sphere::getMinZ() { return this->pos.z - this->radius; }





// TRIANGLE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================

// Null constructor
Triangle::Triangle(void) {
	a = glm::vec3(0, 0, 0);
	b = glm::vec3(0, 0, 0);
	c = glm::vec3(0, 0, 0);
}

// Basic Constructor
Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _colour ) {
	a = _a;
	b = _b;
	c = _c;
	colour = _colour;
}

//=======================================================================================
// Intersection
//   checks intersection of a triangle and returns true if it does.
//   I have used Möller–Trumbore intersection algorithm.
//=======================================================================================
bool Triangle::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	const float EPSILON = 0.0000001;
	float x, f, u1, v1;
	glm::vec3 h, s, q;

	glm::vec3 edge1 = b - a;
	glm::vec3 edge2 = c - a;

	h = glm::cross(_rayDirection, edge2);
	x = glm::dot(edge1, h);
	if (x > -EPSILON && x < EPSILON)
		return false;

	f = 1.0 / x;
	s = _rayOrigin - a;
	u = f * (glm::dot(s, h));
	if (u < 0.0 || u > 1.0)
		return false;

	q = glm::cross(s, edge1);
	v = f * (glm::dot(_rayDirection, q));
	if (v < 0.0 || u + v > 1.0)
		return false;
	w = 1.0f - u - v;
	// By now we can surely compute t* to check where the intersection point is on the line.
	float t0 = f * (glm::dot(edge2, q));
	if (t0 > EPSILON) {   // Ray intersection in this case
		*t = t0;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection
		return false;
}


//====================================================================================
// centroid
//    get coordinates of centroid of the triangle
//====================================================================================
glm::vec3 Triangle::centroid() {
	float x = (this->a.x + this->b.x + this->c.x) / 3.0f;
	float y = (this->a.y + this->b.y + this->c.y) / 3.0f;
	float z = (this->a.z + this->b.z + this->c.z) / 3.0f;
	return glm::vec3(x, y, z);
}


// Calculation for normal on 
float getNormalX(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return ( A.y * B.z ) - ( A.z * B.y ); 
}
float getNormalY(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return ( A.z * B.x ) - ( A.x * B.z );
}
float getNormalZ(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return ( A.x * B.y ) - ( A.y * B.x ); 
}

// Calculating triangle's normal
glm::vec3 getNormal( Triangle *triangle ) {
	float triNormX = getNormalX( triangle->a, triangle->b, triangle->c );
	float triNormY = getNormalY( triangle->a, triangle->b, triangle->c );
	float triNormZ = getNormalZ( triangle->a, triangle->b, triangle->c );
	return glm::vec3(triNormX, triNormY, triNormZ);
}

glm::vec3 Triangle::calNormal(glm::vec3 _p0) {
	float normalX = getNormalX(this->a, this->b, this->c);
	float normalY = getNormalY(this->a, this->b, this->c);
	float normalZ = getNormalZ(this->a, this->b, this->c);
	//getNormal(this);
	glm::vec3(1, 0, 0);

	glm::vec3 edge0 = glm::normalize(b - a);
	glm::vec3 edge1 = glm::normalize(c - b);
	return glm::normalize(glm::cross(edge0, edge1));
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Triangle::getMaxX() { return glm::max(glm::max(this->a.x, this->b.x), this->c.x); }
float Triangle::getMaxY() { return glm::max(glm::max(this->a.y, this->b.y), this->c.y); }
float Triangle::getMaxZ() { return glm::max(glm::max(this->a.z, this->b.z), this->c.z); }
float Triangle::getMinX() { return glm::min(glm::min(this->a.x, this->b.x), this->c.x); }
float Triangle::getMinY() { return glm::min(glm::min(this->a.y, this->b.y), this->c.y); }
float Triangle::getMinZ() { return glm::min(glm::min(this->a.z, this->b.z), this->c.z); }





// BOX INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Box::Box(void) {
	bounds[0] = glm::vec3(0);
	bounds[1] = glm::vec3(0);
	colour = options.backgroundColour;
}


Box::Box(glm::vec3  b0, glm::vec3  b1, glm::vec3 _colour ) { 
	bounds[0] = b0;
	bounds[1] = b1;
	colour = _colour;
}

bool Box::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	glm::vec3 invDir = 1.0f / _rayDirection;           // Inverse ray direction
	int sign[3];
	sign[0] = (invDir.x < 0);
	sign[1] = (invDir.y < 0);
	sign[2] = (invDir.z < 0);

	tmin = (bounds[sign[0]].x - _rayOrigin.x) * invDir.x;
	tmax = (bounds[1 - sign[0]].x - _rayOrigin.x) * invDir.x;
	tymin = (bounds[sign[1]].y - _rayOrigin.y) * invDir.y;
	tymax = (bounds[1 - sign[1]].y - _rayOrigin.y) * invDir.y;

	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[sign[2]].z - _rayOrigin.z) * invDir.z;
	tzmax = (bounds[1 - sign[2]].z - _rayOrigin.z) * invDir.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	*t = tmin;
	if (*t < 0) {
		*t = tmax;
		if (*t < 0) {
			return false;
		}
	}
	return true;
}

glm::vec3 Box::calNormal(glm::vec3 _p0) {
	N = getNormalPlane(_p0);
	return N;
}

glm::vec3 Box::getNormalPlane(glm::vec3 _p0) {
	glm::vec3 toReturn = glm::vec3(0, 0, 0);
	if (_p0.z+0.0005f > bounds[1].z && _p0.z-0.0005f < bounds[1].z ) // point intersected with the front of the box.
		toReturn = glm::vec3(0, 0, 1);
	else if (_p0.z + 0.0005f > bounds[0].z && _p0.z -0.0005f < bounds[0].z)
		toReturn = glm::vec3(0, 0, -1);
	else if (_p0.x + 0.0005f > bounds[1].x && _p0.x - 0.0005f < bounds[1].x)
		toReturn = glm::vec3(1, 0, 0);
	else if (_p0.x + 0.0005f > bounds[0].x && _p0.x - 0.0005f < bounds[0].x)
		toReturn = glm::vec3(-1, 0, 0);
	else if(_p0.y + 0.0005f > bounds[1].y && _p0.y -0.0005f < bounds[1].y)
		toReturn = glm::vec3(0, 1, 0);
	else if(_p0.y + 0.0005f > bounds[0].y && _p0.y - 0.0005f < bounds[0].y)
		toReturn = glm::vec3(0, -1, 0);
	return toReturn;
}

glm::vec3 Box::centroid() {
	glm::vec3 b0 = this->bounds[0];
	glm::vec3 b1 = this->bounds[1];
	float x = (b1.x + b0.x) / 2;
	float y = (b1.y + b0.y) / 2;
	float z = (b1.z + b0.z) / 2;
	return glm::vec3(x, y, z);
}


//========================================================================================
// getMax/Min
//   This function returns a max/min values for each of x, y and z.
//========================================================================================
float Box::getMaxX() { return glm::max(this->bounds[0].x, this->bounds[1].x); }
float Box::getMaxY() { return glm::max(this->bounds[0].y, this->bounds[1].y); }
float Box::getMaxZ() { return glm::max(this->bounds[0].z, this->bounds[1].z); }
float Box::getMinX() { return glm::min(this->bounds[0].x, this->bounds[1].x); }
float Box::getMinY() { return glm::min(this->bounds[0].y, this->bounds[1].y); }
float Box::getMinZ() { return glm::min(this->bounds[0].z, this->bounds[1].z); }



// SHADOW ATTENUATION
//=======================================================================================
//=======================================================================================
//=======================================================================================
ShadowAtt::ShadowAtt(void) {
	pos = glm::vec3(0, 0, 0);
	size = glm::vec3(0, 0, 0);
}

ShadowAtt::ShadowAtt(glm::vec3 _pos, glm::vec3 _size) {
	pos = _pos;
	size = _size;
}

ShadowAtt::~ShadowAtt() {

}



///=================================================================================
/// Acceleration Structure -> BVH
///
///
///
///=================================================================================

//==================================================================================
// buildBVH()
//  This function uses the global meshArr[] array and makes a list of BBOXes depending
//  on different meshes( Sphere, Box and Triangle ).
//==================================================================================
Node::Node(void) {
	this->bbox = new Box();
	this->left = nullptr;
	this->right = nullptr;
	this->meshList = new list<Mesh*>();
}

void Node::addMesh(Mesh *mesh) { this->meshList->push_back(mesh); }


void Node::printMeshes() {
	Mesh *tempMesh = nullptr;
	list<Mesh*> tempList = *this->meshList;
	cout << " " << endl;
	cout << "======================================================" << endl;
	for (list<Mesh*>::iterator it = tempList.begin(); it != tempList.end(); ++it) {
		tempMesh = &**it;
		cout << tempMesh->num << endl;
	}
	cout << "======================================================" << endl;
}


//===============================================================================
// BVH constructor
//   it creates a new root node inside BVH hierarchy.
//===============================================================================
BVH::BVH(void) {
	this->root = new Node();
	initiateBVH();
	buildBVH(this->root);
}


//===============================================================================
// initiateBVH
//   this function initates BVH structure and set the root node.
//===============================================================================
void BVH::initiateBVH() {
	Mesh *tempMesh = nullptr;
	int index = 0;
	for (list<Mesh*>::iterator it = globalMeshList->begin(); it != globalMeshList->end(); ++it) {
		tempMesh = &**it;
		this->root->addMesh(tempMesh);
	}
}

//===============================================================================
// setBBox
//   this function takes in Node as a parameter and sets a bounding box by finding
//   min pos and max pos out of all the meshes in the list of the given node
//===============================================================================
void BVH::setBBox(Node *curr) {
	list<Mesh*> tempMeshList = *curr->meshList;
	float maxX = tempMeshList.front()->centroid().x, maxY = tempMeshList.front()->centroid().y, maxZ = tempMeshList.front()->centroid().z
		, minX = tempMeshList.front()->centroid().x, minY = tempMeshList.front()->centroid().y, minZ = tempMeshList.front()->centroid().z;
	Mesh *tempMesh;
	Mesh *maxXMesh = tempMeshList.front(), *maxYMesh = tempMeshList.front(), *maxZMesh = tempMeshList.front(), 
		 *minXMesh = tempMeshList.front(), *minYMesh = tempMeshList.front(), *minZMesh = tempMeshList.front();
	for (list<Mesh*>::iterator it = tempMeshList.begin(); it != tempMeshList.end(); ++it) {
		tempMesh = &**it;
		if (tempMesh->getMaxX() > maxX)
			maxX = tempMesh->getMaxX();
		if (tempMesh->getMaxY() > maxY)
			maxY = tempMesh->getMaxY();
		if (tempMesh->getMaxZ() > maxZ)
			maxZ = tempMesh->getMaxZ();

		if (tempMesh->getMinX() < minX)
			minX = tempMesh->getMinX();
		if (tempMesh->getMinY() < minY)
			minY = tempMesh->getMinY();
		if (tempMesh->getMinZ() < minZ)
			minZ = tempMesh->getMinZ();
	}

	glm::vec3 b0 = glm::vec3(minX, minY, minZ);
	glm::vec3 b1 = glm::vec3(maxX, maxY, maxZ);
	curr->bbox = new Box(b0, b1, glm::vec3(0));
}


//===============================================================================
// listSplit
//   -this function is my BVH split heuristic.
//   -The idea is that I find the two objects that are furthest to one another and
//    compute the midpoint of these 2. Then, I split the curr's list into left and
//    right lists.
//===============================================================================
void BVH::listSplit(Node *curr, Node *tempLeft, Node *tempRight) {
	list<Mesh*> tempMeshList = *curr->meshList;
	list<Mesh*> *listLeft = new list<Mesh*>();
	list<Mesh*> *listRight = new list<Mesh*>();
	list<float> *distanceList = new list<float>();
	Box bbox = *curr->bbox;
	glm::vec3 b0 = bbox.bounds[0];
	glm::vec3 b1 = bbox.bounds[1];
	/*cout << "b0: " << b0.x << ", " << b0.y << ", " << b0.z << endl;
	cout << "b1: " << b1.x << ", " << b1.y << ", " << b1.z << endl;*/
	float b0NearDistance = glm::abs(glm::distance(tempMeshList.front()->centroid(), b0));
	float b1NearDistance = glm::abs(glm::distance(tempMeshList.front()->centroid(), b1));
	Mesh *b0NearMesh = tempMeshList.front();
	Mesh *b1NearMesh = tempMeshList.front();
	Mesh *tempMesh = nullptr;
	for (list<Mesh*>::iterator it = tempMeshList.begin(); it != tempMeshList.end(); ++it) {
		tempMesh = &**it;
		float tempb0NearDistance = glm::abs(glm::distance(tempMesh->centroid(), b0));
		float tempb1NearDistance = glm::abs(glm::distance(tempMesh->centroid(), b1));
		if ( tempb0NearDistance < b0NearDistance) {
			b0NearDistance = tempb0NearDistance;
			b0NearMesh = tempMesh;
		}
		if (tempb1NearDistance < b1NearDistance) {
			b1NearDistance = tempb1NearDistance;
			b1NearMesh = tempMesh;
		}
	}

	tempMesh = nullptr;
	for (list<Mesh*>::iterator it = tempMeshList.begin(); it != tempMeshList.end(); ++it) {
		tempMesh = &**it;
		if (abs(glm::distance(b0NearMesh->centroid(), tempMesh->centroid())) < glm::abs(glm::distance(b1NearMesh->centroid(), tempMesh->centroid())))
				listLeft->push_back(tempMesh);
		else 
			listRight->push_back(tempMesh);
	}
	list<Mesh*> leftIt = *listLeft;
	list<Mesh*> righIt = *listRight;
	tempMesh = nullptr;
	tempMesh = nullptr;

	tempLeft->meshList = listLeft;
	tempRight->meshList = listRight;
}


//===============================================================================
// buildBVH
//   this is the main function to build a BVH recursively. When this function is
//   completed, this->root will have the root node of the entire tree.
//===============================================================================
bool BVH::buildBVH(Node *curr) {
	if ( curr->meshList->size() >= 1 )
		setBBox(curr); // This sets the BBOX for the current node
	if (curr->meshList->size() <= 1) // BASE CASE
		return true;
	Node *leftNode = new Node();
	Node *rightNode = new Node();
	curr->left =  leftNode;
	curr->right = rightNode;
	listSplit(curr, leftNode, rightNode);  // so leftNode and rightNode now contains the lists
	if ( curr->left->meshList->size() == 0 || curr->right->meshList->size() == 0) {
		return true;
	}
	this->buildBVH(leftNode);
	return this->buildBVH(rightNode);
}


//===============================================================================
// Intersection
//   returns true if any one of leaves are hit by the ray. False otherwise.
//   
//   If this function returns true, we must perform traceRay. Otherwise, we don't
//   call traceRay to improve the performance.
//===============================================================================
bool BVH::findIntersection(Node *curr, glm::vec3 _rayOrigin, glm::vec3 _rayDirection, bool *isHit) {
	if (curr == NULL )
		return false;
	float t = 0.0f;
	bool hit = curr->bbox->Intersection(_rayOrigin, _rayDirection, &t);
	if (hit) {
		if ( curr->left == nullptr && curr->right == nullptr) {
			*isHit = true;
			return true;
		}
		else {
			findIntersection(curr->left, _rayOrigin, _rayDirection, isHit);
			findIntersection(curr->right, _rayOrigin, _rayDirection, isHit);
		}
	}
	return false;
}

void BVH::traverseTest(Node *curr) {
	if (curr == NULL) {
		return;
	}
	curr->printMeshes();
	traverseTest(curr->left);
	traverseTest(curr->right);
}

// Offset points to move the origin which makes an ugly seam.
#define POINT_OFFSET 3893343

Checkerboard::Checkerboard(void) {
	colour1 = glm::vec3(0.3, 0.2, 0.1);
	colour2 = glm::vec3(0.7, 0.8, 0.9);
	scale = 50;
	shininess = 50;
	reflectivity = 0.7;
}

Checkerboard::Checkerboard(glm::vec3 _colour1, glm::vec3 _colour2, float _scale, float _shininess, float _reflectivity) {
	colour1 = _colour1;
	colour2 = _colour2;
	scale = _scale;
	shininess = _shininess;
	reflectivity = _reflectivity;
}

glm::vec3 Checkerboard::getColour(glm::vec3 point){
	bool x = (int)((point.x + POINT_OFFSET) / scale) % 2 == 0;
	bool y = (int)((point.y + POINT_OFFSET) / scale) % 2 == 0;
	bool z = (int)((point.z + POINT_OFFSET) / scale) % 2 == 0;

	if (x ^ y ^ z) {
		return colour1;
	}
	else {
		return colour2;
	}
}

float Checkerboard::getShininess() {
	return shininess;
}

float Checkerboard::getReflectivity() {
	return reflectivity;
}

