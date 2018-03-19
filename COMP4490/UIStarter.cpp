#include "UIStarter.h"

Options options;
using namespace std;

UIStarter::UIStarter()
{
}


UIStarter::~UIStarter()
{
}


int UIStarter::sceneSelector() {
	int selector = 0;
	cout << "Please specify which scene you would like to render. Press a number. ex) 1 for boxes.ray." << endl;
	cout << "1. boxes.ray" << endl;
	cout << "2. sphere.ray" << endl;
	cout << "3. easy1.ray" << endl;

	cin >> selector;
	return selector;
}

void UIStarter::createScene(list<Mesh*> *meshList, int selector) {
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
}

void UIStarter::createLights(list<Light*> *lightList, int selector) {
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
