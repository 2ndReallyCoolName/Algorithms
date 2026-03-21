#pragma once
#include "Octree.h"
#include <random>
#include <iostream>

std::random_device rd;
std::mt19937 engine(rd());
std::uniform_real_distribution<float> distrib(-40.0f, 40.0f);
std::uniform_real_distribution<float> distribMove(5.0f, 100.0f);

void CreateNodes(std::vector<Oct3Node*>* nodes, std::vector<std::vector<vec3>>& vertices, int numNodes) {
	for (int i = 0; i < numNodes; i++) {
		float x = distrib(engine);
		float y = distrib(engine);
		float z = distrib(engine);
		Oct3Node* node = new Oct3Node(x, y, z, nullptr);
		float s = distrib(engine);
		
		std::vector<vec3> verts(8);
		verts[0] = vec3(x-s, y-s, z-s);
		verts[1] = vec3(x - s, y - s, z + s);
		verts[2] = vec3(x - s, y + s, z - s);
		verts[3] = vec3(x - s, y + s, z + s);
		verts[4] = vec3(x + s, y - s, z - s);
		verts[5] = vec3(x + s, y - s, z + s);
		verts[6] = vec3(x + s, y + s, z - s);
		verts[7] = vec3(x + s, y + s, z + s);
		vertices.push_back(verts);

		node->volume = s * s * s * 8;
		node->collider.setVertices(&vertices.back());
		nodes->push_back(node);
	}
}

void OctTreeInsert(OctTree* t, Oct3Node* node) {
	t->insert(node);
	/*std::cout << "===================================================================================" << std::endl;
	std::cout << "Inserted node at position: (" << node->pos[0] << ", " << node->pos[1] << ", " << node->pos[2] << ")" << std::endl;
	t->printTree();
	std::cout << "===================================================================================" << std::endl;*/
}

void OctTreeSearch(OctTree* t, Oct3Node* node) {
	std::cout << "===================================================================================" << std::endl;
	std::cout << "Search" << std::endl;
	Oct3Node* found = t->search(node);
	if (found != nullptr) {
		std::cout << "Node found at position: (" << found->pos[0] << ", " << found->pos[1] << ", " << found->pos[2] << ")" << std::endl;
	}
	else {
		std::cout << "Node not found in tree" << std::endl;
	}
	std::cout << "===================================================================================" << std::endl;
}

void OctTreeRemove(OctTree* t, Oct3Node* node) {
	std::cout << "===================================================================================" << std::endl;
	std::cout << "Removed node at position: (" << node->pos[0] << ", " << node->pos[1] << ", " << node->pos[2] << ")" << std::endl;
	t->remove(node);
	t->printTree();
	std::cout << "===================================================================================" << std::endl;
}


void OctTreeMove(OctTree* t, Oct3Node* node) {
	std::cout << "===================================================================================" << std::endl;
	std::cout << "Moved node initial position: (" << node->pos[0] << ", " << node->pos[1] << ", " << node->pos[2] << ")" << std::endl;
	node->pos[0] += distribMove(engine);
	node->pos[1] += distribMove(engine);
	node->pos[2] += distribMove(engine);
	t->moveNode(node);
	t->printTree();
	std::cout << "===================================================================================" << std::endl;
}

void OctTreeGetCollisions(OctTree* t, Oct3Node* node) {
	std::cout << "===================================================================================" << std::endl;
	std::cout << "Get Collisions for node at position: (" << node->pos[0] << ", " << node->pos[1] << ", " << node->pos[2] << ")" << std::endl;
	std::vector<Oct3Node*> collisions = t->getCollisions(node);
	if (collisions.empty()) {
		std::cout << "No collisions found" << std::endl;
	}
	else {
		std::cout << "Collisions found with nodes at positions: " << std::endl;
		for (Oct3Node* collision : collisions) {
			std::cout << "(" << collision->pos[0] << ", " << collision->pos[1] << ", " << collision->pos[2] << ")" << std::endl;
			std::cout << "Collider vertices: " << std::endl;
			for (const vec3& vertex : collision->getColliderVertices()) {
				std::cout << "  Vertex: ";
				vertex.print();
			}
		}
	}
	std::cout << "===================================================================================" << std::endl;
}

void Test() {
	float w = 50.0f;
	float h = 50.0f;
	float d = 50.0f;

	OctTree tree(-w, -h, -d, w, h, d);

	std::vector<Oct3Node*> nodes;
	std::vector<std::vector<vec3>> vertices;

	CreateNodes(&nodes, vertices, 2000);

	for (Oct3Node* node : nodes) {
		OctTreeInsert(&tree, node);
	}
	
	tree.draw();
	/*
	for (Oct3Node* node : nodes) {
		OctTreeSearch(t, node);
	}
	for (Oct3Node* node : nodes) {
		OctTreeGetCollisions(t, node);
	}
	for (Oct3Node* node : nodes) {
		OctTreeMove(t, node);
	}
	for (Oct3Node* node : nodes) {
		OctTreeRemove(t, node);
	}*/

}