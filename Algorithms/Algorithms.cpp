// Algorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Octree.h"

void Test(OctTree* t, std::vector<float>* v);


int main()
{
	float w = 10;
	float h = 10;
	float d = 10;

	//std::vector<std::vector<float>> points = { {0,0,0}, {1,0,0}, {1,1,0}, {-1,-1,0}, {-1,-1,-1}, {1,2,0}, {1,4,0}, {1,3,0}, {-1,-1,4}, {-1,-4,-1}, {0,1,2}, {1,2,2}, {1,2, 3}, {-2,-2,0}, {-2,-1,-1} , {2.3, 0.4, 3.4} };
	std::vector<std::vector<float>> points = { {0,0,0}, {1,0,0}, {1,5,0}, {-4,-1,0}, {-5,-6,-1}, {4,2,9}, {2,4,-9}, {-1,-1,0}, {-1,-1,-1}, {1,2,0}, {1,4,0}, {1,3,0}, {-1,-1,4}, {-1,-4,-1}, {0,1,2}, {1,2,2}, {1,2, 3}, {-2,-2,0}, {-2,-1,-1} , {2.3, 0.4, 3.4} };

	OctTree tree(-w, -h, -d, w, h, d);

	for (int i = 0; i < points.size(); i++) {
		tree.insert(points[i]);
	}

	tree.printTree();


	for (std::vector<float> p : points) {
		Test(&tree, &p);
		std::cout << "----------------------------------------------" << std::endl;
	}

	std::vector<float> p = { 10,10,10 };
	Test(&tree, &p);
	p = { -1,1,2 };
	Test(&tree, &p);
	p = { 0.2, 0 , 2 };
	Test(&tree, &p);

	//p = { -1,-1,-1 };
	//remove(p, &root);
	//printTree(&root, "");
	//Test(&root, &p);

	std::vector<std::vector<float>> points2Delete = { {0,0,0}, {1,0,0}, {-4,-1,0}, {-5,-6,-1}, {4,2,9} };
	for (int i = 0; i < points2Delete.size(); i++) {
		std::cout << "Delete===============================================" << std::endl;
		std::cout << "Deleting point: (" << points2Delete[i][0] << ", " << points2Delete[i][1] << ", " << points2Delete[i][2] << ")" << std::endl;
		tree.remove(points2Delete[i]);
		tree.printTree();
	}

	for (std::vector<float> p : points2Delete) {
		Test(&tree, &p);
		std::cout << "----------------------------------------------" << std::endl;
	}

	return 0;
}

void Test(OctTree* t, std::vector<float>* p) {
	std::cout << "Test function called" << std::endl;
	
	if (t->search(p) != nullptr) {
		std::cout << "Found point at node";
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
