#include <stdio.h>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include "a1main.h"
// THINGS TO BE DONE
// 1. Trinagle Normal calcualtion
// 2. Multiple Light sources.
// 3. 


using namespace std;

//GLOBAL VARIABLE
Mesh* meshArr[11];
bool hardShadow = true;

void trace(char *input_file, void *window, int width, int height) {
	renderSI(window, width, height);             // Rendering Sphere Intersection
}

void pick(void *window, int x, int y) {
	unsigned char r, g, b;
	if (get(window, x, y, r, g, b)) {
		printf("pick @ (%d, %d): colour [%d %d %d]\n", x, y, r, g, b);
		set(window, x, y, (13+r)%256, (25+g)%256, (41+b)%256);
		redraw(window);
	}
}

//=================================================================================
//renderSI
//  renders sphere intersection using raytracing.
//
//=================================================================================
void renderSI(void * window, int width, int height)
{
	int count = 0;
	// SPHERE
	meshArr[0] = new Sphere(glm::vec3(0, -10005, -50), glm::vec3(0.1f, 0.1f, 0.1f), 10000); // dark Floor
	meshArr[1] = new Sphere(glm::vec3(5, 5, -15), glm::vec3(1.0f, 0.0f, 0.0f), 1); // RED SHPERE
	meshArr[2] = new Sphere(glm::vec3(6, 0, -15), glm::vec3(1.0f, 0.0f, 0.0f), 1.5); // WHITE SPHERE
	meshArr[3] = new Sphere(glm::vec3(7, 0, -15), glm::vec3(1.0f, 1.0f, 0), 2); // BLUE SPHERE
	meshArr[4] = new Sphere(glm::vec3(-5, 0, -15), glm::vec3(1.0f, 1.0f, 0), 2);  // YELLOW SHPERE

	// TRIANGULAR MESH
	meshArr[5] = new Triangle(glm::vec3(-2, 6, -17), glm::vec3(2, 6, -15), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f)); // EMERALD TRIANGLE // u -> CAP
	meshArr[6] = new Triangle(glm::vec3(2, 6, -15), glm::vec3(-2, 6, -13), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f)); // v -> ABP
	meshArr[7] = new Triangle(glm::vec3(-2, 6, -13), glm::vec3(-2, 6, -17), glm::vec3(0, 2, -15), glm::vec3(0.08f, 0.33f, 0.08f)); // w -> BCP
	meshArr[8] = new Triangle(glm::vec3(2, 6, -15), glm::vec3(-2 , 6, -13), glm::vec3(-2, 6, -17), glm::vec3(0.08f, 0.33f, 0.08f)); // ABC -> base

	// BOXES
	meshArr[9] = new Box(glm::vec3(-5, 3, -13), glm::vec3(-3, 5, -11), glm::vec3(0.20f, 0.20f, 1.0f));
	meshArr[10] = new Box(glm::vec3(-7, -5, -15), glm::vec3(-3, -1, -11), glm::vec3(1.0f, 0.078f, 0.58f));

	// CREATING DIRECTIONAL LIGHT
	ShadowAtt* dirLight = new ShadowAtt(glm::vec3(15, 20, 0), glm::vec3(15.0f, 0.1f, 15.0f));

	glm::vec3 **image = new glm::vec3*[width];
	for (int i = 0; i < width; i++) 
		image[i] = new glm::vec3[height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float pixRemapX = (2 * ((x + 0.5f) / (float)width) - 1)*(float)(width/height) ;
			float pixRemapY = 1 - 2 * ((y + 0.5f) / (float)height);
			float pixCamX = pixRemapX * glm::tan(glm::radians(90.0) / 2);
			float pixCamY = pixRemapY * glm::tan(glm::radians(90.0) / 2);
			glm::vec3 PcamSpace = glm::vec3(-pixRemapX, pixRemapY, 1);
			glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
			glm::vec3 rayDirection = glm::normalize(rayOrigin - PcamSpace);

			float minT = INFINITY;
			int sphereHit = -1;
			float t0 = 0.0f;

			for (int i = 0; i < sizeof(meshArr) / sizeof(meshArr[0]); i++) {
				bool hit = meshArr[i]->Intersection(rayOrigin, rayDirection, &t0);

				if (hit && t0 < minT) {
					minT = t0;
					sphereHit = i;
				}
			}

			if (sphereHit != -1) {
				glm::vec3 p0, lightPos, lightSize, lightCentre, diffuseColour, specularColour, normal, ambient,
					lightRay, diffuse, reflection, specular;	
				diffuseColour = glm::vec3(0, 0, 0);
				specularColour = glm::vec3(0, 0, 0);
				glm::vec3 phongShade = glm::vec3(0);
				glm::vec3 ambientShade = glm::vec3(0);
				float attenuation;
				int shininess = 0;
				glm::vec3 lightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
				for (int i = 0; i < 1; i++) {
					p0 = rayOrigin + (minT * rayDirection);
					// LIGHT PROPERTIES 1 -> POINT LIGHT
					if ( i == 0) {
						lightPos = glm::vec3(15, 20, 0);
					}
					// LIGHT PROPERTIES 2 -> POINT LIGHT
					else if( i == 1 ){
						lightPos = glm::vec3(-15, 20, 0);
					}


					// NORMAL -> to be used for lighting
					normal = meshArr[sphereHit]->calNormal(&shininess, p0, &diffuseColour, &specularColour);

					// AMBIENT LIGHTING
					ambient = meshArr[sphereHit]->colour * glm::vec3(0.1, 0.1, 0.1);

					// DIFFUSE LIGHTING
					lightRay = glm::normalize(lightPos - p0);
					//lightRay = glm::normalize(lightPos); // for directional light
					diffuse = diffuseColour * lightIntensity * glm::max(0.0f, glm::dot(lightRay, normal));

					// SPECULAR LIGHTING
					reflection = glm::normalize(2 * (glm::dot(lightRay, normal)) * normal - lightRay);
					float maxCalc = glm::max(0.0f, glm::dot(reflection, glm::normalize(rayOrigin - p0)));
					specular = specularColour * lightIntensity * glm::pow(maxCalc, shininess);

					// DISTANCE ANNUTATION
					float distance = glm::distance(lightPos, p0);
					attenuation = 1.0f / (0.005 + 0.0025*distance + 0.0010*distance*distance);


					// CHECK IF THE LIGHT HITS THE MESHES
					int lightHitsMesh = -1;
					if (hardShadow) {
						for (int j = 0; j < sizeof(meshArr) / sizeof(meshArr[0]); j++) {
							bool lightHit = meshArr[j]->Intersection(p0 + (1e-4f * normal), lightRay, &t0);
							if (lightHit && (t0 < minT)) {
								minT = t0;
								lightHitsMesh = j;
							}
						}

						ambientShade = ambientShade+ (meshArr[lightHitsMesh]->colour * ambient);
						phongShade = phongShade + (specular + diffuse)*attenuation;
						if (i == 0) {
							// IT DOES NOT INTERSECT AT SOME POINT
							if (lightHitsMesh != -1) {
								ambientShade = setToOrigRGB(ambientShade);
								set(window, x, y, ambientShade.x, ambientShade.y, ambientShade.z);
							}
							else {
								count++;
								phongShade = setToOrigRGB(phongShade);
								//cout << phongShade.x << ", " << phongShade.y << ", " << phongShade.z << endl;
								set(window, x, y, phongShade.x, phongShade.y, phongShade.z);
							}
						}
					}
			
				}
			}
			else
			{
				set(window, x, y, 255, 255, 255);
			}
		}
	}
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


// MESH DEFINITION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Mesh::Mesh(void) {
	pos = glm::vec3(0, 0, 0);
	colour = glm::vec3(0, 0, 0);
	N = glm::vec3(0, 0, 0);  // Normal
}

Mesh::Mesh(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _N) {
	pos = _position;
	colour = _colour;
	N = _N;
}


bool Mesh::Intersection(glm::vec3 _rayOrigin, glm::vec3 _rayDirection, float *t) {
	return false;
}

glm::vec3 Mesh::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	return N;
}



// SPHERE INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
Sphere::Sphere(void) {
	pos = glm::vec3(0, 0, 0);
	radius = 0;
	colour = glm::vec3(0, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, glm::vec3 _colour, float _radius) {
	pos = _position;
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

glm::vec3 Sphere::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	*_shininess = 155;  // to give a glow effect
	*_diffuse = colour;
	*_specular = glm::vec3(0.8, 0.8, 0.8);
	N = glm::normalize(_p0 - pos);
	return N;
}



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

glm::vec3 Triangle::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	*_shininess = 100;
	*_diffuse = colour;
	*_specular = glm::vec3(0.5,0.5, 0.5);

	float normalX = getNormalX(this->a, this->b, this->c);
	float normalY = getNormalY(this->a, this->b, this->c);
	float normalZ = getNormalZ(this->a, this->b, this->c);
	getNormal(this);
	glm::vec3(1, 0, 0);
	return glm::vec3(1, 0, 0);
}



// BOX INTERSECTION
//=======================================================================================
//=======================================================================================
//=======================================================================================
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

glm::vec3 Box::calNormal(int *_shininess, glm::vec3 _p0, glm::vec3 *_diffuse, glm::vec3 *_specular) {
	*_shininess = 50;  // to give a glow effect
	*_diffuse = colour;
	*_specular = glm::vec3(0.8, 0.8, 0.8);
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

/*else {
float sample = 2.0f;
float totalRays = sample * sample;
float uniformX = lightSize.x / sample;
float uniformZ = lightSize.z / sample;
float raysHit = 1.0f;
for (float m = 0; m < lightSize.x; m += uniformX) {
for (float n = 0; n < lightSize.z; n += uniformZ) {
float t0s = 0.0f;
float minTs = INFINITY;
lightHitsMesh = -1;

lightPos = glm::vec3(m, lightPos.y, n);
lightRay = glm::normalize(lightPos - p0);
for (int l = 0; l < sizeof(meshArr) / sizeof(meshArr[0]); l++) {
if (sphereHit != l) {
bool lightingHit = meshArr[l]->Intersection(p0 + (1e-4f * normal), lightRay, &t0s);
if (lightingHit && t0s < minTs) {
minTs = t0s;
lightHitsMesh = l;
}
}
}

if (lightHitsMesh = -1)
raysHit = raysHit + (1 / totalRays);
}
}
glm::vec3 phongShade = glm::vec3((raysHit) * (diffuse + specular));
phongShade = setToOrigRGB(phongShade);
set(window, x, y, phongShade.x, phongShade.y, phongShade.z);
}*/