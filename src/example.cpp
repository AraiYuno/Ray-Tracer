﻿#include <stdio.h>
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

