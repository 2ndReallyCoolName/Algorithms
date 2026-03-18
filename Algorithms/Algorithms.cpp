// Algorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GJK.h"



int main()
{
	std::vector<vec3> verticesA = { {-1, 1, 0}, {-2, 1, 0}, {-1, -1, 0}, {-2, -1, 0}, {-1, 1, 1}, {-2, 1, 1}, {-1, -1, 1}, {-2, -1, 1} };
	std::vector<vec3> verticesB = { {-1.5, 0, 0.5}, {0, 1.5, 0.5}, {0, -1.5, 0.5}, {1.5, 0, 0.5},  {-1.5, 0, -0.5}, {0, 1.5, -0.5}, {0, -1.5, -0.5}, {1.5, 0, -0.5} };
	MeshCollider colliderA(verticesA);
	MeshCollider colliderB(verticesB);


	GJK(colliderA, colliderB) ? std::cout << "Colliders intersect" << std::endl : std::cout << "Colliders do not intersect" << std::endl;

	verticesA = { {-1, 1, 0}, {-2, 1, 0}, {-1, -1, 0}, {-2, -1, 0}, {-1, 1, 1}, {-2, 1, 1}, {-1, -1, 1}, {-2, -1, 1} };
	verticesB = { {-0.5, 0, 0.5}, {0, 1.5, 0.5}, {0, -1.5, 0.5}, {1.5, 0, 0.5},  {-0.5, 0, -0.5}, {0, 1.5, -0.5}, {0, -1.5, -0.5}, {1.5, 0, -0.5} };

	colliderA = MeshCollider(verticesA);
	colliderB = MeshCollider(verticesB);

	GJK(colliderA, colliderB) ? std::cout << "Colliders intersect" << std::endl : std::cout << "Colliders do not intersect" << std::endl;

	return 0;
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
