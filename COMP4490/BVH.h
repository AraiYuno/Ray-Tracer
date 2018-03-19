#include "Node.h"

#pragma once
class BVH {
public:
	Node * root;
	// This function takes in all the objects and creates a BVH tree.
	BVH(void);
	void initiateBVH(list<Mesh*> *meshList);
	void setBBox(Node *curr);
	void listSplit(Node *curr, Node *tempLeft, Node *tempRight);
	void traverseTest(Node *curr);
	bool buildBVH(Node *curr);
	bool findIntersection(Node *curr, glm::vec3 _rayOrigin, glm::vec3 _rayDirection, bool *isHit);
};
