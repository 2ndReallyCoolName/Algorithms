#pragma once
#include <vector>
#include <string>
#include <iostream>

int h =5 , w = 5, d = 5;

#define TopLeftFront  0
#define TopRightFront  1
#define BottomLeftFront  2
#define BottomRightFront  3
#define TopLeftBack  4
#define TopRightBack 5
#define BottomLeftBack  6
#define BottomRightBack  7


struct Node {
	Node* parent;
	std::string  name;
	std::string type;
	std::vector<float>* point;
	std::vector<float> topLeftFront;
	std::vector<float> bottomRightBack;
	std::vector<Node*> children;

	Node(Node* parent) : 
		parent(parent)
	{
		point = nullptr;
		type = "leaf";
	}

	Node(Node* parent, std::string name, std::vector<float>* p, std::vector<float> b1, std::vector<float> b2) :
		parent(parent), name(name), point(p), topLeftFront(b1), bottomRightBack(b2) {
		type = "internal";
		children.reserve(8);
		for (int i = 0; i < 8; i++) {
			children.push_back(new Node(this));
			children[children.size() - 1]->name = name;
		}
	}

	~Node() {
		for(Node* n : children) {
			delete n;
		}
	}
};


Node root = {
	nullptr, // parent
	"0", // name,
	nullptr, // point
	std::vector<float>({(float)-w, (float)h, (float)d}), // topLeftFront
	std::vector<float>({(float)w, (float)-h, (float)-d}), // bottomRightBack
};


void insert(std::vector<float>& p, Node* n) {
	
	float x = (n->topLeftFront[0] + n->bottomRightBack[0]) / 2;
	float y = (n->topLeftFront[1] + n->bottomRightBack[1]) / 2;
	float z = (n->topLeftFront[2] + n->bottomRightBack[2]) / 2;

	int pos = -1;
	if (p[0] >= x) {
		if (p[1] >= y) {
			if (p[2] >= z) {
				pos = TopRightFront;
			}
			else {
				pos = TopRightBack;
			}
		}
		else {
			if (p[2] >= z) {
				pos = BottomRightFront;
			}
			else {
				pos = BottomRightBack;
			}
		}
	}
	else {
		if (p[1] >= y) {
			if (p[2] >= z) {
				pos = TopLeftFront;
			}
			else {
				pos = TopLeftBack;
			}
		}
		else {
			if (p[2] >= z) {
				pos = BottomLeftFront;
			}
			else {
				pos = BottomLeftBack;
			}
		}
	}

	// Base case
	if (n->children[pos]->type == "leaf" && n->children[pos]->point == nullptr) {
		n->children[pos]->point = &p;
		n->children[pos]->name = n->name;
		return;
	}

	if (n->children[pos]->type == "internal") {
		insert(p, n->children[pos]);
	}
	else {
		std::vector<float> lp = *n->children[pos]->point;
		delete n->children[pos];
		switch (pos) {
		case TopLeftFront:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name)+1), nullptr, n->topLeftFront, std::vector<float>({ x, y, z}));
			break;
		case TopRightFront:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ x, n->topLeftFront[1], n->topLeftFront[2] }), std::vector<float>({n->bottomRightBack[0], y, z}));
			break;
		case BottomLeftFront:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({n->topLeftFront[0], y, n->topLeftFront[2] }), std::vector<float>({  x, n->bottomRightBack[1], z }));
			break;
		case BottomRightFront:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ x, y, n->topLeftFront[2] }), std::vector<float>({ n->bottomRightBack[0], n->bottomRightBack[1], z }));
			break;
		case TopLeftBack:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ n->topLeftFront[0], n->topLeftFront[1],  z }), std::vector<float>({ x, y, n->bottomRightBack[2]}));
			break;
		case TopRightBack:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ x, n->topLeftFront[1],  z }), std::vector<float>({ n->bottomRightBack[0], y, n->bottomRightBack[2] }));
			break;
		case BottomLeftBack:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ n->topLeftFront[0], y,  z }), std::vector<float>({ x, n->bottomRightBack[1], n->bottomRightBack[2] }));
			break;
		case BottomRightBack:
			n->children[pos] = new Node(n, std::to_string(std::stoi(n->name) + 1), nullptr, std::vector<float>({ x, y,  z }), n->bottomRightBack);
			break;
		default:
			std::cout << "Weird result" << std::endl;
			return;
		}
		insert(p, n->children[pos]);
	}
}

void printTree(Node* n, std::string acc) {


	std::cout << acc + " " + n->name + " " + n->type;
	if (n->type == "internal") {
		std::vector<float> p = n->topLeftFront;
		std::cout << "(" + std::to_string(p[0]) + "," + std::to_string(p[1]) + "," + std::to_string(p[2]) << ")" ;
		p = n->bottomRightBack;
		std::cout << "(" + std::to_string(p[0]) + "," + std::to_string(p[1]) + "," + std::to_string(p[2]) << ")" << ";" << std::endl;
	}
	if (n->point != nullptr) {
		std::vector<float> p = *(n->point);
		std::cout << "(" + std::to_string(p[0]) + "," + std::to_string(p[1]) + "," + std::to_string(p[2]) << ")" << std::endl;
	}
	else {
		std::cout << std::endl;
	}
	for (Node* child : n->children) {
		printTree(child, acc + "-");
	}
}