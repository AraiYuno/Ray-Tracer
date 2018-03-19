#include <list>
#include "Mesh.h"
#include "Box.h"

using namespace std;

// Node to be used in BVH. I am using binary tree structure for my BVH
class Node {
public:
	Node(void);
	// Mutators
	void addMesh(Mesh* mesh);
	Box *bbox;
	Node *left, *right;
	list<Mesh*> *meshList;

	void printMeshes();
};
