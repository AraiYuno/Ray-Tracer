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
		set(window, x, y, (13 + r) % 256, (25 + g) % 256, (41 + b) % 256);
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
	cout << "Please keep in mind that my scenes will look different as this is hard-coded scenes approved by the professor." << endl;
	cout << "Please specify which scene you would like to render. Press a number. ex) 1 for boxes.ray." << endl;
	cout << "1. boxes.ray" << endl;
	cout << "2. sphere.ray" << endl;
	cout << "3. easy1.ray" << endl;
	cout << "4. shell.ray" << endl;
	cout << "5. hihchcock.ray" << endl;
	cout << "6. reflection.ray" << endl;
	cout << "7. sphere_box.ray" << endl;


	cin >> selector;
	return selector;
}

//=================================================================================
//renderSI
//  renders sphere intersection using raytracing.
//
//=================================================================================
void renderSI(void * window, int width, int height)
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
		box3->setSurfaceProperties(76.8f, 1.5f, glm::vec3(box3->colour), glm::vec3(0, 0.9, 0.4), glm::vec3(0.2f), glm::vec3(0.0f), glm::vec3(0, 0.9, 0.4));
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
	else if (selector == 4)
		createShell(globalMeshList);
	else if (selector == 5) {
		Sphere *sphere1 = new Sphere(glm::vec3(0, 0, -35), glm::vec3(0.8, 0.2, 0.4), 2.0f);
		sphere1->setSurfaceProperties(0.0f, 1.0f, sphere1->colour, glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.f), glm::vec3(0.f));
		meshList->push_back(sphere1);
		createhitchcock(globalMeshList);
	}
	else if (selector == 6) {
		Sphere *sphere3 = new Sphere(glm::vec3(-3.5, -0.522035, -3.43002), glm::vec3(0.34, 0.07, 0.56), 0.6);
		sphere3->setSurfaceProperties(122.074f, 1.3f, sphere3->colour, glm::vec3(0.7), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.4, 0.4, 0.4));
		meshList->push_back(sphere3);

		Sphere *sphere4 = new Sphere(glm::vec3(2.560763, 1.08614, -5.70474), glm::vec3(0.56, 0.24, 0.12), 2);
		sphere4->setSurfaceProperties(120.889f, 1.3f, sphere4->colour, glm::vec3(0.7), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.4, 0.4, 0.4));
		meshList->push_back(sphere4);

		Sphere *sphere5 = new Sphere(glm::vec3(0, -105, -20), glm::vec3(0.1f, 0.1f, 0.1f), 100);
		sphere5->setSurfaceProperties(122.074f, 1.3f, sphere5->colour, glm::vec3(0.7), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0));
		meshList->push_back(sphere5);

		Sphere *sphere1 = new Sphere(glm::vec3(0.507093, 0.337143, -6.72743), glm::vec3(0, 0.4, 0.4), 1);
		sphere1->setSurfaceProperties(102.4, 1.3f, sphere1->colour, glm::vec3(0.8), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(1.0));
		sphere1->surfaceMaterial = REFLECTION;
		meshList->push_back(sphere1);

		Sphere *sphere2 = new Sphere(glm::vec3(-1.85934, 0.188571, -5.84847), glm::vec3(0, 0.4, 0.4), 1.5);
		sphere2->setSurfaceProperties(115.2f, 1.3f, sphere2->colour, glm::vec3(0.8), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(1.0));
		sphere2->surfaceMaterial = REFLECTION;
		meshList->push_back(sphere2);
	}
	else if (selector == 7) {
		Box *box1 = new Box(glm::vec3(-0.2, -0.5, -3.5), glm::vec3(0.6, 0.3, 0.5), glm::vec3(0.8, 0.8, 0.3));
		box1->setSurfaceProperties(76.8f, 1.3f, glm::vec3(box1->colour), glm::vec3(0.0, 0.8, 0), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.0f, 0.9f, 0.0f));
		box1->surfaceMaterial = REFLECTION_AND_REFRACTION;
		meshList->push_back(box1);

		Sphere *sphere1 = new Sphere(glm::vec3(0, 0, -1), glm::vec3(0.5, 0.5, 0.9), 0.707107);
		sphere1->setSurfaceProperties(102.4, 1.3f, sphere1->colour, glm::vec3(0.8), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(1.0));
		box1->surfaceMaterial = REFLECTION_AND_REFRACTION;
		meshList->push_back(sphere1);
	}

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
	else if (selector == 4) {
		options.constant_attenuation = 0.25f;
		options.linear_attenuation = 0.00337241f;
		options.quadratic_attenuation = 4.5492e-05f;
		lightList->push_back(new PointLight(glm::vec3(-9.27109, 1.67651, 4.81528), glm::vec3(1, 1, 1)));
		lightList->push_back(new DirLight(glm::vec3(0.349707, 2.11712, -6.04645), glm::vec3(1, 1, 1)));
	}
	else if (selector == 5) {
		options.fov = 30;
		lightList->push_back(new DirLight(glm::vec3(-1, -1, -1), glm::vec3(0.7f)));
	}
	else if (selector == 6) {
		options.fov = 45;
		options.constant_attenuation = 0.25f;
		options.linear_attenuation = 0.00337241f;
		options.quadratic_attenuation = 4.5492e-05f;
		lightList->push_back(new PointLight(glm::vec3(-4.90189, 15, -5.37518), glm::vec3(1)));
		lightList->push_back(new PointLight(glm::vec3(5.304256, 15, 0), glm::vec3(1)));
		lightList->push_back(new DirLight(glm::vec3(0.169031, 0.565714, -8.07967), glm::vec3(1.0f)));
	}
	else if (selector == 7) {
		options.fov = 45;
		lightList->push_back(new DirLight(glm::vec3(0, 0, -3), glm::vec3(1.0f)));
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
			hitColour = ((kr*13.0f) * castRay(reflectionRayOrig, reflectionDirection, depth + 1));
			break;
		}
		case REFLECTION_AND_REFRACTION:
		{
			float reflec = 0.2, trans = 0.8;
			glm::vec3 refractionDirection = glm::normalize(refract(_rayDirection, N, &hitMesh->ior, &reflec, &trans));
			glm::vec3 refractionRayOrig = (glm::dot(refractionDirection, N) < 0) ?
				p0 - N * options.bias :
				p0 + N * options.bias;
			float kr;
			fresnel(_rayDirection, N, &hitMesh->ior, &kr);
			hitColour = castRay(refractionRayOrig, refractionDirection, depth + 1);
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


glm::vec3 refract(const glm::vec3 &_I, glm::vec3 &_N, const float *ior, float *_reflc, float *_trans )
{
	/*float cosi = -glm::dot(_N, _I);
	float etat = *ior;
	float sinT2 = etat * etat * (1.0f - cosi * cosi);
	float cosT = sqrt(1.0f - sinT2);
	return _I * etat + _N * (_N * cosi - cosT);*/
	
	/*float cosi = clamp(-1, 1, glm::dot(_I, _N));
	float etai = 1, etat = *ior;
	glm::vec3 n = _N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -_N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? glm::vec3(0) : eta * _I + (eta * cosi - sqrtf(k)) * n;*/
	float n1, n2, n;
	glm::vec3 normal = _N;
	float cosI = glm::dot(_I, _N);
	if (cosI > 0.0)
	{
		n1 = *ior;
		n2 = 1.0;
		_N = glm::vec3(normal.z, normal.y, normal.x);//invert
	}
	else
	{
		n1 = 1.0;
		n2 = *ior;
		cosI = -cosI;
	}
	n = n1 / n2;
	float sinT2 = n * n * (1.0 - cosI * cosI);
	float cosT = sqrt(1.0 - sinT2);
	//fresnel equations
	float rn = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	float rt = (n2 * cosI - n1 * cosT) / (n2 * cosI + n2 * cosT);
	rn *= rn;
	rt *= rt;
	*_reflc = (rn + rt)*0.5;
	*_trans = 1.0 - *_reflc;
	if (n == 1.0)
		return _I;
	if (cosT*cosT < 0.0)//tot inner refl
	{
		*_reflc = 1;
		*_trans = 0;
		return reflect(_I, normal);
	}
	return n * _I + (n * cosI - cosT) *_N;
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



// Ray DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Ray::Ray(const glm::vec3 _rayOrig, const glm::vec3 _rayDir) {
	this->rayOrig = _rayOrig;
	this->rayDir = _rayDir;
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
			float thc = glm::sqrt((radius*radius) - ((glm::dot(L, L)) - (tca * tca))); // Thc = sqrt( radius^2 -  d^2 )using pythagorean algorithm
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
Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _colour) {
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
	return (A.y * B.z) - (A.z * B.y);
}
float getNormalY(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return (A.z * B.x) - (A.x * B.z);
}
float getNormalZ(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c) {
	glm::vec3 A = _b - _a;
	glm::vec3 B = _c - _a;
	return (A.x * B.y) - (A.y * B.x);
}

// Calculating triangle's normal
glm::vec3 getNormal(Triangle *triangle) {
	float triNormX = getNormalX(triangle->a, triangle->b, triangle->c);
	float triNormY = getNormalY(triangle->a, triangle->b, triangle->c);
	float triNormZ = getNormalZ(triangle->a, triangle->b, triangle->c);
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


Box::Box(glm::vec3  b0, glm::vec3  b1, glm::vec3 _colour) {
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
	if (_p0.z + 0.0005f > bounds[1].z && _p0.z - 0.0005f < bounds[1].z) // point intersected with the front of the box.
		toReturn = glm::vec3(0, 0, 1);
	else if (_p0.z + 0.0005f > bounds[0].z && _p0.z - 0.0005f < bounds[0].z)
		toReturn = glm::vec3(0, 0, -1);
	else if (_p0.x + 0.0005f > bounds[1].x && _p0.x - 0.0005f < bounds[1].x)
		toReturn = glm::vec3(1, 0, 0);
	else if (_p0.x + 0.0005f > bounds[0].x && _p0.x - 0.0005f < bounds[0].x)
		toReturn = glm::vec3(-1, 0, 0);
	else if (_p0.y + 0.0005f > bounds[1].y && _p0.y - 0.0005f < bounds[1].y)
		toReturn = glm::vec3(0, 1, 0);
	else if (_p0.y + 0.0005f > bounds[0].y && _p0.y - 0.0005f < bounds[0].y)
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
		if (tempb0NearDistance < b0NearDistance) {
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
	if (curr->meshList->size() >= 1)
		setBBox(curr); // This sets the BBOX for the current node
	if (curr->meshList->size() <= 1) // BASE CASE
		return true;
	Node *leftNode = new Node();
	Node *rightNode = new Node();
	curr->left = leftNode;
	curr->right = rightNode;
	listSplit(curr, leftNode, rightNode);  // so leftNode and rightNode now contains the lists
	if (curr->left->meshList->size() == 0 || curr->right->meshList->size() == 0) {
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
	if (curr == NULL)
		return false;
	float t = 0.0f;
	bool hit = curr->bbox->Intersection(_rayOrigin, _rayDirection, &t);
	if (hit) {
		if (curr->left == nullptr && curr->right == nullptr) {
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

glm::vec3 Checkerboard::getColour(glm::vec3 point) {
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


///////////////////////////////////////////////////////////////////////////
   // HARD CODED SCENES //
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
Triangle *createTriangle(float p1[3], float p2[3], float p3[3], glm::vec3 _colour) {
	return new Triangle(glm::vec3(p1[0], p1[1], p1[2]), glm::vec3(p2[0], p2[1], p2[2]), glm::vec3(p3[0], p3[1], p3[2]), _colour);
}

Box *createBox(float p[3], float scale, glm::vec3 _colour) {
	return new Box(glm::vec3(p[0] - scale, p[1] - scale, p[2] - scale), glm::vec3(p[0] + scale, p[1] + scale, p[2] + scale), _colour);
}


void createShell(list<Mesh*> *meshList) {
	float points[1068][3] = { { 1.8362,-1.2945,-6.07054 },{ 1.99172,-0.912005,-5.75835 },{ 1.48876,-1.19121,-5.23106 },{ 1.36694,-1.555,-5.57858 },{ 1.8362,-1.2945,-6.07054 },{ 1.48876,-1.19121,-5.23106 },{ 1.99172,-0.912005,-5.75835 },{ 1.91364,-0.445838,-5.5473 },{ 1.44437,-0.706346,-5.05533 },{ 1.48876,-1.19121,-5.23106 },{ 1.99172,-0.912005,-5.75835 },{ 1.44437,-0.706346,-5.05533 },{ 1.91364,-0.445838,-5.5473 },{ 1.62289,-0.0209029,-5.49394 },{ 1.24567,-0.230311,-5.09848 },{ 1.44437,-0.706346,-5.05533 },{ 1.91364,-0.445838,-5.5473 },{ 1.24567,-0.230311,-5.09848 },{ 1.62289,-0.0209029,-5.49394 },{ 1.19738,0.248931,-5.61258 },{ 0.945898,0.109324,-5.34893 },{ 1.24567,-0.230311,-5.09848 },{ 1.62289,-0.0209029,-5.49394 },{ 0.945898,0.109324,-5.34893 },{ 1.19738,0.248931,-5.61258 },{ 0.751116,0.291375,-5.87142 },{ 0.625376,0.221572,-5.73959 },{ 0.945898,0.109324,-5.34893 },{ 1.19738,0.248931,-5.61258 },{ 0.625376,0.221572,-5.73959 },{ 0.751116,0.291375,-5.87142 },{ 0.40368,0.0950424,-6.20111 },{ 0.369989,0.0763384,-6.16578 },{ 0.625376,0.221572,-5.73959 },{ 0.751116,0.291375,-5.87142 },{ 0.369989,0.0763384,-6.16578 },{ 0.40368,0.0950424,-6.20111 },{ 0.248167,-0.287449,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.369989,0.0763384,-6.16578 },{ 0.40368,0.0950424,-6.20111 },{ 0.248167,-0.287449,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.326245,-0.753615,-6.72435 },{ 0.292554,-0.772319,-6.68903 },{ 0.248167,-0.287449,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.292554,-0.772319,-6.68903 },{ 0.326245,-0.753615,-6.72435 },{ 0.616995,-1.17854,-6.7777 },{ 0.491255,-1.24834,-6.64588 },{ 0.292554,-0.772319,-6.68903 },{ 0.326245,-0.753615,-6.72435 },{ 0.491255,-1.24834,-6.64588 },{ 0.616995,-1.17854,-6.7777 },{ 1.04251,-1.44838,-6.65907 },{ 0.791028,-1.58799,-6.39542 },{ 0.491255,-1.24834,-6.64588 },{ 0.616995,-1.17854,-6.7777 },{ 0.791028,-1.58799,-6.39542 },{ 1.04251,-1.44838,-6.65907 },{ 1.48877,-1.49082,-6.40023 },{ 1.11155,-1.70023,-6.00476 },{ 0.791028,-1.58799,-6.39542 },{ 1.04251,-1.44838,-6.65907 },{ 1.11155,-1.70023,-6.00476 },{ 1.48877,-1.49082,-6.40023 },{ 1.8362,-1.2945,-6.07054 },{ 1.36694,-1.555,-5.57858 },{ 1.11155,-1.70023,-6.00476 },{ 1.48877,-1.49082,-6.40023 },{ 1.36694,-1.555,-5.57858 },{ 1.36694,-1.555,-5.57858 },{ 1.48876,-1.19121,-5.23106 },{ 0.796927,-1.33284,-4.89898 },{ 0.721451,-1.68714,-5.26874 },{ 1.36694,-1.555,-5.57858 },{ 0.796927,-1.33284,-4.89898 },{ 1.48876,-1.19121,-5.23106 },{ 1.44437,-0.706346,-5.05533 },{ 0.798885,-0.838479,-4.7455 },{ 0.796927,-1.33284,-4.89898 },{ 1.48876,-1.19121,-5.23106 },{ 0.798885,-0.838479,-4.7455 },{ 1.44437,-0.706346,-5.05533 },{ 1.24567,-0.230311,-5.09848 },{ 0.726799,-0.336527,-4.84941 },{ 0.798885,-0.838479,-4.7455 },{ 1.44437,-0.706346,-5.05533 },{ 0.726799,-0.336527,-4.84941 },{ 1.24567,-0.230311,-5.09848 },{ 0.945898,0.109324,-5.34893 },{ 0.599982,0.038514,-5.18289 },{ 0.726799,-0.336527,-4.84941 },{ 1.24567,-0.230311,-5.09848 },{ 0.599982,0.038514,-5.18289 },{ 0.945898,0.109324,-5.34893 },{ 0.625376,0.221572,-5.73959 },{ 0.452418,0.186166,-5.65657 },{ 0.599982,0.038514,-5.18289 },{ 0.945898,0.109324,-5.34893 },{ 0.452418,0.186166,-5.65657 },{ 0.625376,0.221572,-5.73959 },{ 0.369989,0.0763384,-6.16578 },{ 0.323645,0.0668602,-6.14353 },{ 0.452418,0.186166,-5.65657 },{ 0.625376,0.221572,-5.73959 },{ 0.323645,0.0668602,-6.14353 },{ 0.369989,0.0763384,-6.16578 },{ 0.248167,-0.287449,-6.5133 },{ 0.248168,-0.287441,-6.51329 },{ 0.323645,0.0668602,-6.14353 },{ 0.369989,0.0763384,-6.16578 },{ 0.248168,-0.287441,-6.51329 },{ 0.248167,-0.287449,-6.5133 },{ 0.292554,-0.772319,-6.68903 },{ 0.24621,-0.781796,-6.66678 },{ 0.248168,-0.287441,-6.51329 },{ 0.248167,-0.287449,-6.5133 },{ 0.24621,-0.781796,-6.66678 },{ 0.292554,-0.772319,-6.68903 },{ 0.491255,-1.24834,-6.64588 },{ 0.318297,-1.28375,-6.56286 },{ 0.24621,-0.781796,-6.66678 },{ 0.292554,-0.772319,-6.68903 },{ 0.318297,-1.28375,-6.56286 },{ 0.491255,-1.24834,-6.64588 },{ 0.791028,-1.58799,-6.39542 },{ 0.445113,-1.6588,-6.22938 },{ 0.318297,-1.28375,-6.56286 },{ 0.491255,-1.24834,-6.64588 },{ 0.445113,-1.6588,-6.22938 },{ 0.791028,-1.58799,-6.39542 },{ 1.11155,-1.70023,-6.00476 },{ 0.592677,-1.80644,-5.7557 },{ 0.445113,-1.6588,-6.22938 },{ 0.791028,-1.58799,-6.39542 },{ 0.592677,-1.80644,-5.7557 },{ 1.11155,-1.70023,-6.00476 },{ 1.36694,-1.555,-5.57858 },{ 0.721451,-1.68714,-5.26874 },{ 0.592677,-1.80644,-5.7557 },{ 1.11155,-1.70023,-6.00476 },{ 0.721451,-1.68714,-5.26874 },{ 0.721451,-1.68714,-5.26874 },{ 0.796927,-1.33284,-4.89898 },{ 0.0215533,-1.3153,-4.81266 },{ -0.00198408,-1.67078,-5.1882 },{ 0.721451,-1.68714,-5.26874 },{ 0.0215533,-1.3153,-4.81266 },{ 0.796927,-1.33284,-4.89898 },{ 0.798885,-0.838479,-4.7455 },{ 0.0754513,-0.822113,-4.66496 },{ 0.0215533,-1.3153,-4.81266 },{ 0.796927,-1.33284,-4.89898 },{ 0.0754513,-0.822113,-4.66496 },{ 0.798885,-0.838479,-4.7455 },{ 0.726799,-0.336527,-4.84941 },{ 0.145267,-0.323379,-4.78468 },{ 0.0754513,-0.822113,-4.66496 },{ 0.798885,-0.838479,-4.7455 },{ 0.145267,-0.323379,-4.78468 },{ 0.726799,-0.336527,-4.84941 },{ 0.599982,0.038514,-5.18289 },{ 0.212295,0.0472825,-5.13974 },{ 0.145267,-0.323379,-4.78468 },{ 0.726799,-0.336527,-4.84941 },{ 0.212295,0.0472825,-5.13974 },{ 0.599982,0.038514,-5.18289 },{ 0.452418,0.186166,-5.65657 },{ 0.258574,0.190552,-5.635 },{ 0.212295,0.0472825,-5.13974 },{ 0.599982,0.038514,-5.18289 },{ 0.258574,0.190552,-5.635 },{ 0.452418,0.186166,-5.65657 },{ 0.323645,0.0668602,-6.14353 },{ 0.271704,0.0680282,-6.13776 },{ 0.258574,0.190552,-5.635 },{ 0.452418,0.186166,-5.65657 },{ 0.271704,0.0680282,-6.13776 },{ 0.323645,0.0668602,-6.14353 },{ 0.248168,-0.287441,-6.51329 },{ 0.248167,-0.287449,-6.5133 },{ 0.271704,0.0680282,-6.13776 },{ 0.323645,0.0668602,-6.14353 },{ 0.248167,-0.287449,-6.5133 },{ 0.248168,-0.287441,-6.51329 },{ 0.24621,-0.781796,-6.66678 },{ 0.194269,-0.780634,-6.661 },{ 0.248167,-0.287449,-6.5133 },{ 0.248168,-0.287441,-6.51329 },{ 0.194269,-0.780634,-6.661 },{ 0.24621,-0.781796,-6.66678 },{ 0.318297,-1.28375,-6.56286 },{ 0.124452,-1.27938,-6.54128 },{ 0.194269,-0.780634,-6.661 },{ 0.24621,-0.781796,-6.66678 },{ 0.124452,-1.27938,-6.54128 },{ 0.318297,-1.28375,-6.56286 },{ 0.445113,-1.6588,-6.22938 },{ 0.0574244,-1.65004,-6.18622 },{ 0.124452,-1.27938,-6.54128 },{ 0.318297,-1.28375,-6.56286 },{ 0.0574244,-1.65004,-6.18622 },{ 0.445113,-1.6588,-6.22938 },{ 0.592677,-1.80644,-5.7557 },{ 0.0111454,-1.7933,-5.69096 },{ 0.0574244,-1.65004,-6.18622 },{ 0.445113,-1.6588,-6.22938 },{ 0.0111454,-1.7933,-5.69096 },{ 0.592677,-1.80644,-5.7557 },{ 0.721451,-1.68714,-5.26874 },{ -0.00198408,-1.67078,-5.1882 },{ 0.0111454,-1.7933,-5.69096 },{ 0.592677,-1.80644,-5.7557 },{ -0.00198408,-1.67078,-5.1882 },{ -0.00198408,-1.67078,-5.1882 },{ 0.0215533,-1.3153,-4.81266 },{ -0.719321,-1.14128,-4.98525 },{ -0.693233,-1.50841,-5.34922 },{ -0.00198408,-1.67078,-5.1882 },{ -0.719321,-1.14128,-4.98525 },{ 0.0215533,-1.3153,-4.81266 },{ 0.0754513,-0.822113,-4.66496 },{ -0.615793,-0.659759,-4.82599 },{ -0.719321,-1.14128,-4.98525 },{ 0.0215533,-1.3153,-4.81266 },{ -0.615793,-0.659759,-4.82599 },{ 0.0754513,-0.822113,-4.66496 },{ 0.145267,-0.323379,-4.78468 },{ -0.410384,-0.192857,-4.91413 },{ -0.615793,-0.659759,-4.82599 },{ 0.0754513,-0.822113,-4.66496 },{ -0.410384,-0.192857,-4.91413 },{ 0.145267,-0.323379,-4.78468 },{ 0.212295,0.0472825,-5.13974 },{ -0.158136,0.134292,-5.22605 },{ -0.410384,-0.192857,-4.91413 },{ 0.145267,-0.323379,-4.78468 },{ -0.158136,0.134292,-5.22605 },{ 0.212295,0.0472825,-5.13974 },{ 0.258574,0.190552,-5.635 },{ 0.0733609,0.23405,-5.67816 },{ -0.158136,0.134292,-5.22605 },{ 0.212295,0.0472825,-5.13974 },{ 0.0733609,0.23405,-5.67816 },{ 0.258574,0.190552,-5.635 },{ 0.271704,0.0680282,-6.13776 },{ 0.222078,0.0796778,-6.14933 },{ 0.0733609,0.23405,-5.67816 },{ 0.258574,0.190552,-5.635 },{ 0.222078,0.0796778,-6.14933 },{ 0.271704,0.0680282,-6.13776 },{ 0.248167,-0.287449,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.222078,0.0796778,-6.14933 },{ 0.271704,0.0680282,-6.13776 },{ 0.248166,-0.287458,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.194269,-0.780634,-6.661 },{ 0.144636,-0.768987,-6.67256 },{ 0.248166,-0.287458,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.144636,-0.768987,-6.67256 },{ 0.194269,-0.780634,-6.661 },{ 0.124452,-1.27938,-6.54128 },{ -0.0607733,-1.23588,-6.58442 },{ 0.144636,-0.768987,-6.67256 },{ 0.194269,-0.780634,-6.661 },{ -0.0607733,-1.23588,-6.58442 },{ 0.124452,-1.27938,-6.54128 },{ 0.0574244,-1.65004,-6.18622 },{ -0.313022,-1.56304,-6.27251 },{ -0.0607733,-1.23588,-6.58442 },{ 0.124452,-1.27938,-6.54128 },{ -0.313022,-1.56304,-6.27251 },{ 0.0574244,-1.65004,-6.18622 },{ 0.0111454,-1.7933,-5.69096 },{ -0.544518,-1.66279,-5.82039 },{ -0.313022,-1.56304,-6.27251 },{ 0.0574244,-1.65004,-6.18622 },{ -0.544518,-1.66279,-5.82039 },{ 0.0111454,-1.7933,-5.69096 },{ -0.00198408,-1.67078,-5.1882 },{ -0.693233,-1.50841,-5.34922 },{ -0.544518,-1.66279,-5.82039 },{ 0.0111454,-1.7933,-5.69096 },{ -0.693233,-1.50841,-5.34922 },{ -0.693233,-1.50841,-5.34922 },{ -0.719321,-1.14128,-4.98525 },{ -1.3129,-0.837265,-5.39047 },{ -1.24705,-1.22478,-5.72731 },{ -0.693233,-1.50841,-5.34922 },{ -1.3129,-0.837265,-5.39047 },{ -0.719321,-1.14128,-4.98525 },{ -0.615793,-0.659759,-4.82599 },{ -1.16961,-0.37611,-5.20407 },{ -1.3129,-0.837265,-5.39047 },{ -0.719321,-1.14128,-4.98525 },{ -1.16961,-0.37611,-5.20407 },{ -0.615793,-0.659759,-4.82599 },{ -0.410384,-0.192857,-4.91413 },{ -0.855562,0.0351439,-5.21804 },{ -1.16961,-0.37611,-5.20407 },{ -0.615793,-0.659759,-4.82599 },{ -0.855562,0.0351439,-5.21804 },{ -0.410384,-0.192857,-4.91413 },{ -0.158136,0.134292,-5.22605 },{ -0.454919,0.286293,-5.42866 },{ -0.855562,0.0351439,-5.21804 },{ -0.410384,-0.192857,-4.91413 },{ -0.454919,0.286293,-5.42866 },{ -0.158136,0.134292,-5.22605 },{ 0.0733609,0.23405,-5.67816 },{ -0.0750278,0.310049,-5.77946 },{ -0.454919,0.286293,-5.42866 },{ -0.158136,0.134292,-5.22605 },{ -0.0750278,0.310049,-5.77946 },{ 0.0733609,0.23405,-5.67816 },{ 0.222078,0.0796778,-6.14933 },{ 0.182319,0.100041,-6.17647 },{ -0.0750278,0.310049,-5.77946 },{ 0.0733609,0.23405,-5.67816 },{ 0.182319,0.100041,-6.17647 },{ 0.222078,0.0796778,-6.14933 },{ 0.248166,-0.287458,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.182319,0.100041,-6.17647 },{ 0.222078,0.0796778,-6.14933 },{ 0.248166,-0.287458,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.144636,-0.768987,-6.67256 },{ 0.104871,-0.748621,-6.69971 },{ 0.248166,-0.287458,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.104871,-0.748621,-6.69971 },{ 0.144636,-0.768987,-6.67256 },{ -0.0607733,-1.23588,-6.58442 },{ -0.209172,-1.15988,-6.68573 },{ 0.104871,-0.748621,-6.69971 },{ 0.144636,-0.768987,-6.67256 },{ -0.209172,-1.15988,-6.68573 },{ -0.0607733,-1.23588,-6.58442 },{ -0.313022,-1.56304,-6.27251 },{ -0.609815,-1.41103,-6.47513 },{ -0.209172,-1.15988,-6.68573 },{ -0.0607733,-1.23588,-6.58442 },{ -0.609815,-1.41103,-6.47513 },{ -0.313022,-1.56304,-6.27251 },{ -0.544518,-1.66279,-5.82039 },{ -0.989706,-1.43478,-6.12431 },{ -0.609815,-1.41103,-6.47513 },{ -0.313022,-1.56304,-6.27251 },{ -0.989706,-1.43478,-6.12431 },{ -0.544518,-1.66279,-5.82039 },{ -0.693233,-1.50841,-5.34922 },{ -1.24705,-1.22478,-5.72731 },{ -0.989706,-1.43478,-6.12431 },{ -0.544518,-1.66279,-5.82039 },{ -1.24705,-1.22478,-5.72731 },{ -1.24705,-1.22478,-5.72731 },{ -1.3129,-0.837265,-5.39047 },{ -1.66883,-0.449564,-5.96663 },{ -1.57913,-0.863049,-6.26488 },{ -1.24705,-1.22478,-5.72731 },{ -1.66883,-0.449564,-5.96663 },{ -1.3129,-0.837265,-5.39047 },{ -1.16961,-0.37611,-5.20407 },{ -1.5017,-0.0143821,-5.74163 },{ -1.66883,-0.449564,-5.96663 },{ -1.3129,-0.837265,-5.39047 },{ -1.5017,-0.0143821,-5.74163 },{ -1.16961,-0.37611,-5.20407 },{ -0.855562,0.0351439,-5.21804 },{ -1.12251,0.325916,-5.65016 },{ -1.5017,-0.0143821,-5.74163 },{ -1.16961,-0.37611,-5.20407 },{ -1.12251,0.325916,-5.65016 },{ -0.855562,0.0351439,-5.21804 },{ -0.454919,0.286293,-5.42866 },{ -0.632889,0.480143,-5.71673 },{ -1.12251,0.325916,-5.65016 },{ -0.855562,0.0351439,-5.21804 },{ -0.632889,0.480143,-5.71673 },{ -0.454919,0.286293,-5.42866 },{ -0.0750278,0.310049,-5.77946 },{ -0.164016,0.406982,-5.92349 },{ -0.632889,0.480143,-5.71673 },{ -0.454919,0.286293,-5.42866 },{ -0.164016,0.406982,-5.92349 },{ -0.0750278,0.310049,-5.77946 },{ 0.182319,0.100041,-6.17647 },{ 0.158473,0.126019,-6.21506 },{ -0.164016,0.406982,-5.92349 },{ -0.0750278,0.310049,-5.77946 },{ 0.158473,0.126019,-6.21506 },{ 0.182319,0.100041,-6.17647 },{ 0.248166,-0.287458,-6.5133 },{ 0.248167,-0.287449,-6.5133 },{ 0.158473,0.126019,-6.21506 },{ 0.182319,0.100041,-6.17647 },{ 0.248167,-0.287449,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.104871,-0.748621,-6.69971 },{ 0.0810341,-0.722639,-6.7383 },{ 0.248167,-0.287449,-6.5133 },{ 0.248166,-0.287458,-6.5133 },{ 0.0810341,-0.722639,-6.7383 },{ 0.104871,-0.748621,-6.69971 },{ -0.209172,-1.15988,-6.68573 },{ -0.298144,-1.06294,-6.82977 },{ 0.0810341,-0.722639,-6.7383 },{ 0.104871,-0.748621,-6.69971 },{ -0.298144,-1.06294,-6.82977 },{ -0.209172,-1.15988,-6.68573 },{ -0.609815,-1.41103,-6.47513 },{ -0.787767,-1.21717,-6.76321 },{ -0.298144,-1.06294,-6.82977 },{ -0.209172,-1.15988,-6.68573 },{ -0.787767,-1.21717,-6.76321 },{ -0.609815,-1.41103,-6.47513 },{ -0.989706,-1.43478,-6.12431 },{ -1.25664,-1.144,-6.55644 },{ -0.787767,-1.21717,-6.76321 },{ -0.609815,-1.41103,-6.47513 },{ -1.25664,-1.144,-6.55644 },{ -0.989706,-1.43478,-6.12431 },{ -1.24705,-1.22478,-5.72731 },{ -1.57913,-0.863049,-6.26488 },{ -1.25664,-1.144,-6.55644 },{ -0.989706,-1.43478,-6.12431 },{ -1.57913,-0.863049,-6.26488 },{ -1.57913,-0.863049,-6.26488 },{ -1.66883,-0.449564,-5.96663 },{ -1.73291,-0.0371869,-6.62603 },{ -1.63892,-0.478289,-6.8801 },{ -1.57913,-0.863049,-6.26488 },{ -1.73291,-0.0371869,-6.62603 },{ -1.66883,-0.449564,-5.96663 },{ -1.5017,-0.0143821,-5.74163 },{ -1.56149,0.370378,-6.35685 },{ -1.73291,-0.0371869,-6.62603 },{ -1.66883,-0.449564,-5.96663 },{ -1.56149,0.370378,-6.35685 },{ -1.5017,-0.0143821,-5.74163 },{ -1.12251,0.325916,-5.65016 },{ -1.17058,0.635211,-6.1447 },{ -1.56149,0.370378,-6.35685 },{ -1.5017,-0.0143821,-5.74163 },{ -1.17058,0.635211,-6.1447 },{ -1.12251,0.325916,-5.65016 },{ -0.632889,0.480143,-5.71673 },{ -0.664941,0.68634,-6.04642 },{ -1.17058,0.635211,-6.1447 },{ -1.12251,0.325916,-5.65016 },{ -0.664941,0.68634,-6.04642 },{ -0.632889,0.480143,-5.71673 },{ -0.164016,0.406982,-5.92349 },{ -0.180046,0.51008,-6.08834 },{ -0.664941,0.68634,-6.04642 },{ -0.632889,0.480143,-5.71673 },{ -0.180046,0.51008,-6.08834 },{ -0.164016,0.406982,-5.92349 },{ 0.158473,0.126019,-6.21506 },{ 0.154175,0.153653,-6.25922 },{ -0.180046,0.51008,-6.08834 },{ -0.164016,0.406982,-5.92349 },{ 0.154175,0.153653,-6.25922 },{ 0.158473,0.126019,-6.21506 },{ 0.248167,-0.287449,-6.5133 },{ 0.248168,-0.287441,-6.51329 },{ 0.154175,0.153653,-6.25922 },{ 0.158473,0.126019,-6.21506 },{ 0.248168,-0.287441,-6.51329 },{ 0.248167,-0.287449,-6.5133 },{ 0.0810341,-0.722639,-6.7383 },{ 0.0767487,-0.695006,-6.78247 },{ 0.248168,-0.287441,-6.51329 },{ 0.248167,-0.287449,-6.5133 },{ 0.0767487,-0.695006,-6.78247 },{ 0.0810341,-0.722639,-6.7383 },{ -0.298144,-1.06294,-6.82977 },{ -0.314154,-0.959839,-6.99462 },{ 0.0767487,-0.695006,-6.78247 },{ 0.0810341,-0.722639,-6.7383 },{ -0.314154,-0.959839,-6.99462 },{ -0.298144,-1.06294,-6.82977 },{ -0.787767,-1.21717,-6.76321 },{ -0.819794,-1.01098,-7.09291 },{ -0.314154,-0.959839,-6.99462 },{ -0.298144,-1.06294,-6.82977 },{ -0.819794,-1.01098,-7.09291 },{ -0.787767,-1.21717,-6.76321 },{ -1.25664,-1.144,-6.55644 },{ -1.30469,-0.834717,-7.05099 },{ -0.819794,-1.01098,-7.09291 },{ -0.787767,-1.21717,-6.76321 },{ -1.30469,-0.834717,-7.05099 },{ -1.25664,-1.144,-6.55644 },{ -1.57913,-0.863049,-6.26488 },{ -1.63892,-0.478289,-6.8801 },{ -1.30469,-0.834717,-7.05099 },{ -1.25664,-1.144,-6.55644 },{ -1.63892,-0.478289,-6.8801 },{ -1.63892,-0.478289,-6.8801 },{ -1.73291,-0.0371869,-6.62603 },{ -1.49539,0.337098,-7.26825 },{ -1.4173,-0.12908,-7.47931 },{ -1.63892,-0.478289,-6.8801 },{ -1.49539,0.337098,-7.26825 },{ -1.73291,-0.0371869,-6.62603 },{ -1.56149,0.370378,-6.35685 },{ -1.33988,0.719592,-6.95606 },{ -1.49539,0.337098,-7.26825 },{ -1.73291,-0.0371869,-6.62603 },{ -1.33988,0.719592,-6.95606 },{ -1.56149,0.370378,-6.35685 },{ -1.17058,0.635211,-6.1447 },{ -0.99245,0.915926,-6.62638 },{ -1.33988,0.719592,-6.95606 },{ -1.56149,0.370378,-6.35685 },{ -0.99245,0.915926,-6.62638 },{ -1.17058,0.635211,-6.1447 },{ -0.664941,0.68634,-6.04642 },{ -0.54619,0.873492,-6.36754 },{ -0.99245,0.915926,-6.62638 },{ -1.17058,0.635211,-6.1447 },{ -0.54619,0.873492,-6.36754 },{ -0.664941,0.68634,-6.04642 },{ -0.180046,0.51008,-6.08834 },{ -0.120674,0.603666,-6.24889 },{ -0.54619,0.873492,-6.36754 },{ -0.664941,0.68634,-6.04642 },{ -0.120674,0.603666,-6.24889 },{ -0.180046,0.51008,-6.08834 },{ 0.154175,0.153653,-6.25922 },{ 0.170082,0.178737,-6.30224 },{ -0.120674,0.603666,-6.24889 },{ -0.180046,0.51008,-6.08834 },{ 0.170082,0.178737,-6.30224 },{ 0.154175,0.153653,-6.25922 },{ 0.248168,-0.287441,-6.51329 },{ 0.248169,-0.287432,-6.51329 },{ 0.170082,0.178737,-6.30224 },{ 0.154175,0.153653,-6.25922 },{ 0.248169,-0.287432,-6.51329 },{ 0.248168,-0.287441,-6.51329 },{ 0.0767487,-0.695006,-6.78247 },{ 0.0926647,-0.669927,-6.82548 },{ 0.248169,-0.287432,-6.51329 },{ 0.248168,-0.287441,-6.51329 },{ 0.0926647,-0.669927,-6.82548 },{ 0.0767487,-0.695006,-6.78247 },{ -0.314154,-0.959839,-6.99462 },{ -0.254764,-0.866262,-7.15516 },{ 0.0926647,-0.669927,-6.82548 },{ 0.0767487,-0.695006,-6.78247 },{ -0.254764,-0.866262,-7.15516 },{ -0.314154,-0.959839,-6.99462 },{ -0.819794,-1.01098,-7.09291 },{ -0.701024,-0.823836,-7.41401 },{ -0.254764,-0.866262,-7.15516 },{ -0.314154,-0.959839,-6.99462 },{ -0.701024,-0.823836,-7.41401 },{ -0.819794,-1.01098,-7.09291 },{ -1.30469,-0.834717,-7.05099 },{ -1.12654,-0.554002,-7.53265 },{ -0.701024,-0.823836,-7.41401 },{ -0.819794,-1.01098,-7.09291 },{ -1.12654,-0.554002,-7.53265 },{ -1.30469,-0.834717,-7.05099 },{ -1.63892,-0.478289,-6.8801 },{ -1.4173,-0.12908,-7.47931 },{ -1.12654,-0.554002,-7.53265 },{ -1.30469,-0.834717,-7.05099 },{ -1.4173,-0.12908,-7.47931 },{ -1.4173,-0.12908,-7.47931 },{ -1.49539,0.337098,-7.26825 },{ -0.992424,0.616299,-7.79555 },{ -0.948023,0.131415,-7.97127 },{ -1.4173,-0.12908,-7.47931 },{ -0.992424,0.616299,-7.79555 },{ -1.49539,0.337098,-7.26825 },{ -1.33988,0.719592,-6.95606 },{ -0.870615,0.980096,-7.44804 },{ -0.992424,0.616299,-7.79555 },{ -1.49539,0.337098,-7.26825 },{ -0.870615,0.980096,-7.44804 },{ -1.33988,0.719592,-6.95606 },{ -0.99245,0.915926,-6.62638 },{ -0.615232,1.12534,-7.02186 },{ -0.870615,0.980096,-7.44804 },{ -1.33988,0.719592,-6.95606 },{ -0.615232,1.12534,-7.02186 },{ -0.99245,0.915926,-6.62638 },{ -0.54619,0.873492,-6.36754 },{ -0.294713,1.01311,-6.63119 },{ -0.615232,1.12534,-7.02186 },{ -0.99245,0.915926,-6.62638 },{ -0.294713,1.01311,-6.63119 },{ -0.54619,0.873492,-6.36754 },{ -0.120674,0.603666,-6.24889 },{ 0.00506406,0.673476,-6.38073 },{ -0.294713,1.01311,-6.63119 },{ -0.54619,0.873492,-6.36754 },{ 0.00506406,0.673476,-6.38073 },{ -0.120674,0.603666,-6.24889 },{ 0.170082,0.178737,-6.30224 },{ 0.203773,0.197453,-6.33757 },{ 0.00506406,0.673476,-6.38073 },{ -0.120674,0.603666,-6.24889 },{ 0.203773,0.197453,-6.33757 },{ 0.170082,0.178737,-6.30224 },{ 0.248169,-0.287432,-6.51329 },{ 0.24817,-0.287415,-6.51328 },{ 0.203773,0.197453,-6.33757 },{ 0.170082,0.178737,-6.30224 },{ 0.24817,-0.287415,-6.51328 },{ 0.248169,-0.287432,-6.51329 },{ 0.0926647,-0.669927,-6.82548 },{ 0.12636,-0.651219,-6.86079 },{ 0.24817,-0.287415,-6.51328 },{ 0.248169,-0.287432,-6.51329 },{ 0.12636,-0.651219,-6.86079 },{ 0.0926647,-0.669927,-6.82548 },{ -0.254764,-0.866262,-7.15516 },{ -0.12902,-0.796459,-7.28697 },{ 0.12636,-0.651219,-6.86079 },{ 0.0926647,-0.669927,-6.82548 },{ -0.12902,-0.796459,-7.28697 },{ -0.254764,-0.866262,-7.15516 },{ -0.701024,-0.823836,-7.41401 },{ -0.44954,-0.68424,-7.67764 },{ -0.12902,-0.796459,-7.28697 },{ -0.254764,-0.866262,-7.15516 },{ -0.44954,-0.68424,-7.67764 },{ -0.701024,-0.823836,-7.41401 },{ -1.12654,-0.554002,-7.53265 },{ -0.749318,-0.344609,-7.92811 },{ -0.44954,-0.68424,-7.67764 },{ -0.701024,-0.823836,-7.41401 },{ -0.749318,-0.344609,-7.92811 },{ -1.12654,-0.554002,-7.53265 },{ -1.4173,-0.12908,-7.47931 },{ -0.948023,0.131415,-7.97127 },{ -0.749318,-0.344609,-7.92811 },{ -1.12654,-0.554002,-7.53265 },{ -0.948023,0.131415,-7.97127 },{ -0.948023,0.131415,-7.97127 },{ -0.992424,0.616299,-7.79555 },{ -0.300596,0.757905,-8.12764 },{ -0.302547,0.263535,-8.2811 },{ -0.948023,0.131415,-7.97127 },{ -0.300596,0.757905,-8.12764 },{ -0.992424,0.616299,-7.79555 },{ -0.870615,0.980096,-7.44804 },{ -0.225124,1.11222,-7.75789 },{ -0.300596,0.757905,-8.12764 },{ -0.992424,0.616299,-7.79555 },{ -0.225124,1.11222,-7.75789 },{ -0.870615,0.980096,-7.44804 },{ -0.615232,1.12534,-7.02186 },{ -0.0963546,1.23155,-7.27093 },{ -0.225124,1.11222,-7.75789 },{ -0.870615,0.980096,-7.44804 },{ -0.0963546,1.23155,-7.27093 },{ -0.615232,1.12534,-7.02186 },{ -0.294713,1.01311,-6.63119 },{ 0.0512108,1.08392,-6.79725 },{ -0.0963546,1.23155,-7.27093 },{ -0.615232,1.12534,-7.02186 },{ 0.0512108,1.08392,-6.79725 },{ -0.294713,1.01311,-6.63119 },{ 0.00506406,0.673476,-6.38073 },{ 0.17803,0.708895,-6.46375 },{ 0.0512108,1.08392,-6.79725 },{ -0.294713,1.01311,-6.63119 },{ 0.17803,0.708895,-6.46375 },{ 0.00506406,0.673476,-6.38073 },{ 0.203773,0.197453,-6.33757 },{ 0.250122,0.206952,-6.35981 },{ 0.17803,0.708895,-6.46375 },{ 0.00506406,0.673476,-6.38073 },{ 0.250122,0.206952,-6.35981 },{ 0.203773,0.197453,-6.33757 },{ 0.24817,-0.287415,-6.51328 },{ 0.248171,-0.287407,-6.51327 },{ 0.250122,0.206952,-6.35981 },{ 0.203773,0.197453,-6.33757 },{ 0.248171,-0.287407,-6.51327 },{ 0.24817,-0.287415,-6.51328 },{ 0.12636,-0.651219,-6.86079 },{ 0.172699,-0.641727,-6.88302 },{ 0.248171,-0.287407,-6.51327 },{ 0.24817,-0.287415,-6.51328 },{ 0.172699,-0.641727,-6.88302 },{ 0.12636,-0.651219,-6.86079 },{ -0.12902,-0.796459,-7.28697 },{ 0.0439299,-0.761059,-7.36998 },{ 0.172699,-0.641727,-6.88302 },{ 0.12636,-0.651219,-6.86079 },{ 0.0439299,-0.761059,-7.36998 },{ -0.12902,-0.796459,-7.28697 },{ -0.44954,-0.68424,-7.67764 },{ -0.103635,-0.613437,-7.84367 },{ 0.0439299,-0.761059,-7.36998 },{ -0.12902,-0.796459,-7.28697 },{ -0.103635,-0.613437,-7.84367 },{ -0.44954,-0.68424,-7.67764 },{ -0.749318,-0.344609,-7.92811 },{ -0.230454,-0.238402,-8.17716 },{ -0.103635,-0.613437,-7.84367 },{ -0.44954,-0.68424,-7.67764 },{ -0.230454,-0.238402,-8.17716 },{ -0.749318,-0.344609,-7.92811 },{ -0.948023,0.131415,-7.97127 },{ -0.302547,0.263535,-8.2811 },{ -0.230454,-0.238402,-8.17716 },{ -0.749318,-0.344609,-7.92811 },{ -0.302547,0.263535,-8.2811 },{ -0.302547,0.263535,-8.2811 },{ -0.300596,0.757905,-8.12764 },{ 0.474779,0.740374,-8.21395 },{ 0.420877,0.24717,-8.36164 },{ -0.302547,0.263535,-8.2811 },{ 0.474779,0.740374,-8.21395 },{ -0.300596,0.757905,-8.12764 },{ -0.225124,1.11222,-7.75789 },{ 0.498318,1.09586,-7.83843 },{ 0.474779,0.740374,-8.21395 },{ -0.300596,0.757905,-8.12764 },{ 0.498318,1.09586,-7.83843 },{ -0.225124,1.11222,-7.75789 },{ -0.0963546,1.23155,-7.27093 },{ 0.485191,1.2184,-7.33568 },{ 0.498318,1.09586,-7.83843 },{ -0.225124,1.11222,-7.75789 },{ 0.485191,1.2184,-7.33568 },{ -0.0963546,1.23155,-7.27093 },{ 0.0512108,1.08392,-6.79725 },{ 0.438914,1.07516,-6.84042 },{ 0.485191,1.2184,-7.33568 },{ -0.0963546,1.23155,-7.27093 },{ 0.438914,1.07516,-6.84042 },{ 0.0512108,1.08392,-6.79725 },{ 0.17803,0.708895,-6.46375 },{ 0.371889,0.704527,-6.48534 },{ 0.438914,1.07516,-6.84042 },{ 0.0512108,1.08392,-6.79725 },{ 0.371889,0.704527,-6.48534 },{ 0.17803,0.708895,-6.46375 },{ 0.250122,0.206952,-6.35981 },{ 0.302071,0.205789,-6.36559 },{ 0.371889,0.704527,-6.48534 },{ 0.17803,0.708895,-6.46375 },{ 0.302071,0.205789,-6.36559 },{ 0.250122,0.206952,-6.35981 },{ 0.248171,-0.287407,-6.51327 },{ 0.248172,-0.287398,-6.51327 },{ 0.302071,0.205789,-6.36559 },{ 0.250122,0.206952,-6.35981 },{ 0.248172,-0.287398,-6.51327 },{ 0.248171,-0.287407,-6.51327 },{ 0.172699,-0.641727,-6.88302 },{ 0.224631,-0.642896,-6.88879 },{ 0.248172,-0.287398,-6.51327 },{ 0.248171,-0.287407,-6.51327 },{ 0.224631,-0.642896,-6.88879 },{ 0.172699,-0.641727,-6.88302 },{ 0.0439299,-0.761059,-7.36998 },{ 0.237758,-0.765439,-7.39154 },{ 0.224631,-0.642896,-6.88879 },{ 0.172699,-0.641727,-6.88302 },{ 0.237758,-0.765439,-7.39154 },{ 0.0439299,-0.761059,-7.36998 },{ -0.103635,-0.613437,-7.84367 },{ 0.284034,-0.622208,-7.88681 },{ 0.237758,-0.765439,-7.39154 },{ 0.0439299,-0.761059,-7.36998 },{ 0.284034,-0.622208,-7.88681 },{ -0.103635,-0.613437,-7.84367 },{ -0.230454,-0.238402,-8.17716 },{ 0.35106,-0.251562,-8.24189 },{ 0.284034,-0.622208,-7.88681 },{ -0.103635,-0.613437,-7.84367 },{ 0.35106,-0.251562,-8.24189 },{ -0.230454,-0.238402,-8.17716 },{ -0.302547,0.263535,-8.2811 },{ 0.420877,0.24717,-8.36164 },{ 0.35106,-0.251562,-8.24189 },{ -0.230454,-0.238402,-8.17716 },{ 0.420877,0.24717,-8.36164 },{ 0.420877,0.24717,-8.36164 },{ 0.474779,0.740374,-8.21395 },{ 1.21565,0.566345,-8.04137 },{ 1.11211,0.0848044,-8.20061 },{ 0.420877,0.24717,-8.36164 },{ 1.21565,0.566345,-8.04137 },{ 0.474779,0.740374,-8.21395 },{ 0.498318,1.09586,-7.83843 },{ 1.18958,0.933498,-7.67741 },{ 1.21565,0.566345,-8.04137 },{ 0.474779,0.740374,-8.21395 },{ 1.18958,0.933498,-7.67741 },{ 0.498318,1.09586,-7.83843 },{ 0.485191,1.2184,-7.33568 },{ 1.04087,1.0879,-7.20624 },{ 1.18958,0.933498,-7.67741 },{ 0.498318,1.09586,-7.83843 },{ 1.04087,1.0879,-7.20624 },{ 0.485191,1.2184,-7.33568 },{ 0.438914,1.07516,-6.84042 },{ 0.809377,0.988158,-6.75413 },{ 1.04087,1.0879,-7.20624 },{ 0.485191,1.2184,-7.33568 },{ 0.809377,0.988158,-6.75413 },{ 0.438914,1.07516,-6.84042 },{ 0.371889,0.704527,-6.48534 },{ 0.557129,0.661025,-6.44219 },{ 0.809377,0.988158,-6.75413 },{ 0.438914,1.07516,-6.84042 },{ 0.557129,0.661025,-6.44219 },{ 0.371889,0.704527,-6.48534 },{ 0.302071,0.205789,-6.36559 },{ 0.351715,0.194141,-6.35403 },{ 0.557129,0.661025,-6.44219 },{ 0.371889,0.704527,-6.48534 },{ 0.351715,0.194141,-6.35403 },{ 0.302071,0.205789,-6.36559 },{ 0.248172,-0.287398,-6.51327 },{ 0.248172,-0.28739,-6.51326 },{ 0.351715,0.194141,-6.35403 },{ 0.302071,0.205789,-6.36559 },{ 0.248172,-0.28739,-6.51326 },{ 0.248172,-0.287398,-6.51327 },{ 0.224631,-0.642896,-6.88879 },{ 0.274248,-0.654545,-6.87722 },{ 0.248172,-0.28739,-6.51326 },{ 0.248172,-0.287398,-6.51327 },{ 0.274248,-0.654545,-6.87722 },{ 0.224631,-0.642896,-6.88879 },{ 0.237758,-0.765439,-7.39154 },{ 0.422953,-0.808943,-7.34839 },{ 0.274248,-0.654545,-6.87722 },{ 0.224631,-0.642896,-6.88879 },{ 0.422953,-0.808943,-7.34839 },{ 0.237758,-0.765439,-7.39154 },{ 0.284034,-0.622208,-7.88681 },{ 0.654444,-0.709215,-7.80051 },{ 0.422953,-0.808943,-7.34839 },{ 0.237758,-0.765439,-7.39154 },{ 0.654444,-0.709215,-7.80051 },{ 0.284034,-0.622208,-7.88681 },{ 0.35106,-0.251562,-8.24189 },{ 0.906692,-0.382081,-8.11244 },{ 0.654444,-0.709215,-7.80051 },{ 0.284034,-0.622208,-7.88681 },{ 0.906692,-0.382081,-8.11244 },{ 0.35106,-0.251562,-8.24189 },{ 0.420877,0.24717,-8.36164 },{ 1.11211,0.0848044,-8.20061 },{ 0.906692,-0.382081,-8.11244 },{ 0.35106,-0.251562,-8.24189 },{ 1.11211,0.0848044,-8.20061 },{ 1.11211,0.0848044,-8.20061 },{ 1.21565,0.566345,-8.04137 },{ 1.80924,0.262332,-7.63615 },{ 1.66591,-0.198838,-7.82254 },{ 1.11211,0.0848044,-8.20061 },{ 1.80924,0.262332,-7.63615 },{ 1.21565,0.566345,-8.04137 },{ 1.18958,0.933498,-7.67741 },{ 1.74341,0.649846,-7.29933 },{ 1.80924,0.262332,-7.63615 },{ 1.21565,0.566345,-8.04137 },{ 1.74341,0.649846,-7.29933 },{ 1.18958,0.933498,-7.67741 },{ 1.04087,1.0879,-7.20624 },{ 1.48608,0.859881,-6.90233 },{ 1.74341,0.649846,-7.29933 },{ 1.18958,0.933498,-7.67741 },{ 1.48608,0.859881,-6.90233 },{ 1.04087,1.0879,-7.20624 },{ 0.809377,0.988158,-6.75413 },{ 1.1062,0.836151,-6.55151 },{ 1.48608,0.859881,-6.90233 },{ 1.04087,1.0879,-7.20624 },{ 1.1062,0.836151,-6.55151 },{ 0.809377,0.988158,-6.75413 },{ 0.557129,0.661025,-6.44219 },{ 0.705551,0.585027,-6.34088 },{ 1.1062,0.836151,-6.55151 },{ 0.809377,0.988158,-6.75413 },{ 0.705551,0.585027,-6.34088 },{ 0.557129,0.661025,-6.44219 },{ 0.351715,0.194141,-6.35403 },{ 0.391492,0.173782,-6.32688 },{ 0.705551,0.585027,-6.34088 },{ 0.557129,0.661025,-6.44219 },{ 0.391492,0.173782,-6.32688 },{ 0.351715,0.194141,-6.35403 },{ 0.248172,-0.28739,-6.51326 },{ 0.248173,-0.287381,-6.51326 },{ 0.391492,0.173782,-6.32688 },{ 0.351715,0.194141,-6.35403 },{ 0.248173,-0.287381,-6.51326 },{ 0.248172,-0.28739,-6.51326 },{ 0.274248,-0.654545,-6.87722 },{ 0.313996,-0.674897,-6.85008 },{ 0.248173,-0.287381,-6.51326 },{ 0.248172,-0.28739,-6.51326 },{ 0.313996,-0.674897,-6.85008 },{ 0.274248,-0.654545,-6.87722 },{ 0.422953,-0.808943,-7.34839 },{ 0.571323,-0.884928,-7.24708 },{ 0.313996,-0.674897,-6.85008 },{ 0.274248,-0.654545,-6.87722 },{ 0.571323,-0.884928,-7.24708 },{ 0.422953,-0.808943,-7.34839 },{ 0.654444,-0.709215,-7.80051 },{ 0.951204,-0.861208,-7.59791 },{ 0.571323,-0.884928,-7.24708 },{ 0.422953,-0.808943,-7.34839 },{ 0.951204,-0.861208,-7.59791 },{ 0.654444,-0.709215,-7.80051 },{ 0.906692,-0.382081,-8.11244 },{ 1.35185,-0.610075,-7.80854 },{ 0.951204,-0.861208,-7.59791 },{ 0.654444,-0.709215,-7.80051 },{ 1.35185,-0.610075,-7.80854 },{ 0.906692,-0.382081,-8.11244 },{ 1.11211,0.0848044,-8.20061 },{ 1.66591,-0.198838,-7.82254 },{ 1.35185,-0.610075,-7.80854 },{ 0.906692,-0.382081,-8.11244 },{ 1.66591,-0.198838,-7.82254 },{ 1.66591,-0.198838,-7.82254 },{ 1.80924,0.262332,-7.63615 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.66591,-0.198838,-7.82254 },{ 1.0287,-0.0125276,-7.07471 },{ 1.80924,0.262332,-7.63615 },{ 1.74341,0.649846,-7.29933 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.80924,0.262332,-7.63615 },{ 1.0287,-0.0125276,-7.07471 },{ 1.74341,0.649846,-7.29933 },{ 1.48608,0.859881,-6.90233 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.74341,0.649846,-7.29933 },{ 1.0287,-0.0125276,-7.07471 },{ 1.48608,0.859881,-6.90233 },{ 1.1062,0.836151,-6.55151 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.48608,0.859881,-6.90233 },{ 1.0287,-0.0125276,-7.07471 },{ 1.1062,0.836151,-6.55151 },{ 0.705551,0.585027,-6.34088 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.1062,0.836151,-6.55151 },{ 1.0287,-0.0125276,-7.07471 },{ 0.705551,0.585027,-6.34088 },{ 0.391492,0.173782,-6.32688 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.705551,0.585027,-6.34088 },{ 1.0287,-0.0125276,-7.07471 },{ 0.391492,0.173782,-6.32688 },{ 0.248173,-0.287381,-6.51326 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.391492,0.173782,-6.32688 },{ 1.0287,-0.0125276,-7.07471 },{ 0.248173,-0.287381,-6.51326 },{ 0.313996,-0.674897,-6.85008 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.248173,-0.287381,-6.51326 },{ 1.0287,-0.0125276,-7.07471 },{ 0.313996,-0.674897,-6.85008 },{ 0.571323,-0.884928,-7.24708 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.313996,-0.674897,-6.85008 },{ 1.0287,-0.0125276,-7.07471 },{ 0.571323,-0.884928,-7.24708 },{ 0.951204,-0.861208,-7.59791 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.571323,-0.884928,-7.24708 },{ 1.0287,-0.0125276,-7.07471 },{ 0.951204,-0.861208,-7.59791 },{ 1.35185,-0.610075,-7.80854 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 0.951204,-0.861208,-7.59791 },{ 1.0287,-0.0125276,-7.07471 },{ 1.35185,-0.610075,-7.80854 },{ 1.66591,-0.198838,-7.82254 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.35185,-0.610075,-7.80854 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 },{ 1.0287,-0.0125276,-7.07471 } };

	float x, y, z;
	x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	for (int i = 0; i < 1068; i++) {
			if (i % 3 == 0 & i != 0) {
				Triangle *triangle = createTriangle(points[i - 3], points[i - 2], points[i - 1], glm::vec3(x, y, z));
				triangle->setSurfaceProperties(25.6f, 1.0f, triangle->colour, glm::vec3(0.8), glm::vec3(0.2f), glm::vec3(0.f), glm::vec3(0.f));
				meshList->push_back(triangle);
			}
	}
}


void createhitchcock(list<Mesh*> *meshList) {
	float position[140][3] = { { 4.24406,-0.082272,-50.6479 }, { 2.72672,3.22026,-50.1943 }, { -0.894527,3.89607,-50.0026 }, { -3.47973,2.07571,-49.8341 }, { -3.97895,-1.53007,-50.0924 }, { -0.992903,-4.02906,-50.119 }, { 2.52905,-3.08388,-50.0478 }, { 3.65714,-1.72429,-45.0283 }, { 3.52311,1.43107,-44.7114 }, { 0.792673,3.88397,-45.0208 }, { -2.50091,3.16594,-44.9675 }, { -4.14065,0.295959,-44.9004 }, { -2.62366,-3.00319,-44.8228 }, { 0.828243,-3.76669,-44.8399 }, { 2.58842,-3.08648,-40.0434 }, { 4.18617,-0.028174,-40.0713 }, { 2.63487,2.95231,-39.7597 }, { -0.848422,4.16309,-40.0875 }, { -3.41682,1.33497,-40.1276 }, { -3.8758,-1.48629,-39.8389 }, { -0.712713,-3.80093,-39.9936 }, { 1.06073,-3.86999,-35.0899 }, { 3.75305,-1.79714,-35.021 }, { 3.67367,1.77419,-34.9425 }, { 1.05585,3.99675,-34.8766 }, { -2.55998,3.46732,-35.0494 }, { -4.1829,0.018497,-35.0161 }, { -2.5541,-3.23436,-34.9922 }, { -0.960915,-4.27771,-30.6283 }, { 2.57282,-3.67269,-30.1847 }, { 4.17993,0.0405325,-30.023 }, { 2.62263,3.27833,-29.9919 }, { -0.943986,4.08849,-30.0068 }, { -3.72031,1.74929,-29.96 }, { -3.62187,-1.46583,-29.6949 }, { -2.53198,-3.44274,-25.3143 }, { 1.01243,-4.30212,-25.3548 }, { 3.43609,-1.4983,-24.9176 }, { 3.76823,1.88937,-25.0394 }, { 0.838979,4.09912,-25.0424 }, { -2.73531,3.5856,-25.391 }, { -4.26679,-0.0124775,-25.0306 }, { -3.80258,-1.86014,-19.9655 }, { -1.06504,-4.28307,-20.0499 }, { 2.802,-3.25053,-20.2344 }, { 4.59986,-0.259898,-20.2707 }, { 3.29173,2.96794,-19.89 }, { -0.883654,4.17394,-19.9121 }, { -3.65113,2.08941,-19.7568 }, { -4.38103,-0.0097125,-15.0437 }, { -2.32875,-3.42992,-15.4728 }, { 0.968093,-4.38025,-15.1456 }, { 3.74059,-1.81334,-14.8547 }, { 3.96218,1.77232,-14.893 }, { 1.04342,4.45121,-15.5712 }, { -2.66325,3.13512,-14.9635 }, { -3.91078,1.69236,-10.0873 }, { -3.75934,-2.01729,-10.1158 }, { -1.26108,-4.48399,-10.165 }, { 2.36004,-3.87263,-9.8786 }, { 4.53272,-0.036603,-10.1327 }, { 2.61994,3.46272,-10.0724 }, { -0.951694,4.41541,-10.0857 }, { -2.72152,3.68174,-5.00144 }, { -4.42925,0.068908,-4.92166 }, { -2.71411,-3.46385,-4.99202 }, { 1.0265,-4.44003,-5.1558 }, { 3.8878,-2.27395,-4.83671 }, { 3.90933,1.9621,-5.00612 }, { 1.75372,4.29322,-4.95813 }, { -1.11746,4.18885,-0.044635 }, { -3.64794,2.51888,0.2557 }, { -4.10742,-1.63787,0.237139 }, { -1.26001,-4.64612,-0.475666 }, { 2.94374,-3.41861,-0.293077 }, { 4.21321,-0.210899,0.28801 }, { 2.53508,3.57966,-0.290659 }, { 1.28843,4.68577,4.641 }, { -2.81915,3.43087,5.02786 }, { -4.69696,-0.065256,4.60919 }, { -2.89342,-3.54298,5.02509 }, { 1.09283,-4.50737,4.2026 }, { 4.13821,-1.70924,4.44748 }, { 4.1542,2.06647,4.79947 }, { 3.37534,3.59831,9.7652 }, { -0.94871,4.5299,10.0459 }, { -4.24464,2.19165,9.89919 }, { -4.03725,-2.07776,9.9154 }, { -0.773391,-4.24458,9.97853 }, { 2.65225,-3.79686,10.1594 }, { 4.63136,0.124426,9.77883 }, { 3.83023,2.10961,14.8967 }, { 1.10956,4.48777,15.0944 }, { -2.69782,4.00535,15.0451 }, { -4.67134,0.135121,15.0874 }, { -3.12288,-3.70486,14.8649 }, { 0.583341,-4.84337,15.0074 }, { 4.36944,-2.09891,14.5939 }, { 4.64616,0.048743,19.9998 }, { 2.90698,3.79364,19.7536 }, { -0.868321,4.42161,20.2253 }, { -4.2769,1.89315,19.1805 }, { -4.18748,-1.98036,20.0873 }, { -1.06144,-4.62171,19.961 }, { 2.92873,-3.67759,20.0019 }, { 4.31894,-2.00591,24.9089 }, { 4.13857,2.24233,24.6448 }, { 0.74175,4.48155,24.7646 }, { -2.70457,4.32413,24.7754 }, { -4.83168,0.071362,24.9774 }, { -2.86178,-3.61664,25.0926 }, { 1.18582,-4.46667,24.9192 }, { 2.96601,-3.73781,30.0376 }, { 4.37126,-0.144378,30.2864 }, { 3.31476,3.87402,29.6106 }, { -1.02139,4.59213,30.1016 }, { -4.11707,2.07021,30.1914 }, { -3.91806,-1.9764,30.1365 }, { -1.07684,-4.683,30.0025 }, { 0.771513,-4.73779,35.2188 }, { 4.48201,-1.9965,34.6596 }, { 3.70083,2.03639,34.9485 }, { 1.34993,4.8034,35.0338 }, { -2.97228,4.01362,34.9854 }, { -4.74979,-0.214816,34.7814 }, { -2.63739,-3.70575,34.8912 }, { -0.778993,-4.59118,39.8269 }, { 3.02126,-3.86158,40.0391 }, { 4.60944,-0.184263,40.2787 }, { 3.10526,3.83254,40.0032 }, { -1.14676,4.76746,39.9318 }, { -4.43574,2.16085,39.9864 }, { -4.16294,-2.03123,40.1494 }, { -3.23893,-4.02782,44.4346 }, { 1.04155,-4.58166,45.165 }, { 4.16123,-2.47327,45.2781 }, { 4.50741,2.19639,44.8977 }, { 1.07737,4.73525,45.0758 }, { -3.15817,4.09001,44.7462 }, { -4.94257,0.170861,45.1317 } };
	float y, z;

	for (int i = 0; i < 140; i++) {
		y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		Box *box = createBox(position[i], 0.5f, glm::vec3(0, y, z));
		box->setSurfaceProperties(0.0f, 1.0f, box->colour, glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.f), glm::vec3(0.0f));
		meshList->push_back(box);
	}
}