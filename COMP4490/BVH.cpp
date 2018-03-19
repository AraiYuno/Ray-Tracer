#include "BVH.h"



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
void BVH::initiateBVH(list<Mesh*> *meshList) {
	Mesh *tempMesh = nullptr;
	int index = 0;
	for (list<Mesh*>::iterator it = meshList->begin(); it != meshList->end(); ++it) {
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
