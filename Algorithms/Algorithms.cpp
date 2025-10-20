// Algorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Octree.h"

void Test(Node* n, std::vector<float>* v);


int main()
{
	std::vector<std::vector<float>> points = { {0,0,0}, {1,0,0}, {1,1,0}, {-1,-1,0}, {-1,-1,-1}, {1,2,0}, {1,4,0}, {1,3,0}, {-1,-1,4}, {-1,-4,-1}, {0,1,2}, {1,2,2}, {1,2, 3}, {-2,-2,0}, {-2,-1,-1} , {2.3, 0.4, 3.4} };
	std::cout << root.name << std::endl;

	for (int i = 0; i < points.size(); i++) {
		insert(points[i], &root);
		std::cout << "----------------------------------------------" << std::endl;
	}
	printTree(&root, "");

	for (std::vector<float> p : points) {
		Test(&root, &p);
		std::cout << "----------------------------------------------" << std::endl;
	}

	std::vector<float> p = { 10,10,10 };
	Test(&root, &p);
	p = { -1,1,2 };
	Test(&root, &p);
	p = { 0.2, 0 , 2 };
	Test(&root, &p);

	return 0;
}

void Test(Node* n, std::vector<float>* p) {
	std::cout << "Test function called" << std::endl;
	Node* res = search(n, p);
	if (res != nullptr) {
		std::cout << "Found point at node: " << res->name;
		std::cout << " " << res->type << " " << res->point->at(0) << "," << res->point->at(1) << "," << res->point->at(2) << std::endl;
	}
	else {
		std::cout << "Point not found" << std::endl;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
