#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

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
		std::vector<float>* lp = n->children[pos]->point;
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
		insert(*lp, n->children[pos]);
		insert(p, n->children[pos]);
	}
}

void printNode(Node* n) {
	std::cout << n->name + " " + n->type;
	if (n->type == "internal") {
		std::cout << "(" + std::to_string(n->topLeftFront[0]) + "," + std::to_string(n->topLeftFront[1]) + "," + std::to_string(n->topLeftFront[2]) << ")";
		std::cout << "(" + std::to_string(n->bottomRightBack[0]) + "," + std::to_string(n->bottomRightBack[1]) + "," + std::to_string(n->bottomRightBack[2]) << ")" << std::endl;
	}
	else {
		std::cout << std::endl;
	}
}

void printPoint(std::vector<float>* p, int size) {
	std::cout << "( ";
	for (int i = 0; i < size-1; i++) {
		std::cout << p->at(i) << ", ";
	}
	std::cout << p->at(size - 1) << " )" << std::endl;
}

bool isEqual(float a, float b, float epsilon = 0.00001f) {
	return std::fabs(a - b) <= epsilon;
}

bool compare(std::vector<float>* p1, std::vector<float>* p2) {
	if (p1 == nullptr || p2 == nullptr) return false;

	return (isEqual(p1->at(0), p2->at(0)) && isEqual(p1->at(1), p2->at(1))  && isEqual(p1->at(2), p2->at(2)));
}



Node* search(Node* n, std::vector<float>* p){

	if (n->type == "leaf") {
		if (compare(n->point, p)) {
			return n;
		}
		else {
			return nullptr;
		}
	}
	for (Node* child : n->children) {
		if(child->type == "internal"){
			if (p->at(0) >= child->topLeftFront[0] && p->at(0) <= child->bottomRightBack[0] &&
				p->at(1) >= child->bottomRightBack[1] && p->at(1) <= child->topLeftFront[1] &&
				p->at(2) >= child->bottomRightBack[2] && p->at(2) <= child->topLeftFront[2]) {
				return search(child, p);
			}
		}
		else {

			if (child->point != nullptr && compare(p, child->point)) {
				return child;
			}
		}
	}
	return nullptr;
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
		std::vector<float>* p = n->point;
		std::cout << "(";
		for (int i = 0; i < p->size(); i++) {
			std::cout << p->at(i) << ",";
		}
		std::cout << ")" << std::endl;
		/*std::cout << "(" + std::to_string(p->at(0)) + "," + std::to_string(p->at(1)) + "," + std::to_string(p->at(2)) << ")" << std::endl;*/
		return;
	}
	else {
		std::cout << std::endl;
	}
	for (Node* child : n->children) {
		printTree(child, acc + "-");
	}
}

