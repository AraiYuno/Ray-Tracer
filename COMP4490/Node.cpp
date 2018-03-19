#include "Node.h"

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