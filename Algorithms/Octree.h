#include <vector>
#include <iostream>
#include "GJK.h"
#include <stdexcept>

#define OCT3_TOP_LEFT_FRONT 0
#define OCT3_TOP_RIGHT_FRONT 1
#define OCT3_BOTTOM_LEFT_FRONT 2
#define OCT3_BOTTOM_RIGHT_FRONT 3
#define OCT3_TOP_LEFT_BACK 4
#define OCT3_TOP_RIGHT_BACK 5
#define OCT3_BOTTOM_LEFT_BACK 6
#define OCT3_BOTTOM_RIGHT_BACK 7

struct Oct3Node {
	std::vector<float> pos;
	Oct3Node* parent;
	std::vector<Oct3Node*> children;
	std::vector<float>  topLeftFront;
	std::vector<float>  bottomRightBack;
	float volume = 0.0f;
	int numChildren = 0;
	int index = -1;
	MeshCollider collider;

	Oct3Node(float x, float y, float z, Oct3Node* parent) : pos{ x, y, z }, parent(parent) {
		children.resize(8);
	}
	Oct3Node(float topLeftFrontX, float topLeftFrontY, float topLeftFrontZ, float bottomRightBackX, float bottomRightBackY, float bottomRightBackZ, Oct3Node* parent)
		: parent(parent), topLeftFront({ topLeftFrontX, topLeftFrontY, topLeftFrontZ }), bottomRightBack({ bottomRightBackX, bottomRightBackY, bottomRightBackZ })
	{
		this->volume = (bottomRightBackX - topLeftFrontX) * (bottomRightBackY - topLeftFrontY) * (bottomRightBackZ - topLeftFrontZ);
		children.resize(8);
	}

	MeshCollider getCollider() const {
		return collider;
	}

	const std::vector<vec3>& getColliderVertices() const {
		return collider.getVertices();
	}

	void setColliderVertices(const std::vector<vec3>* vertices) {
		collider.setVertices(vertices);
	}

	void setVolume(float volume) {
		this->volume = volume;
	}

	void add_child(Oct3Node* child, int index) {
		child->parent = this;
		child->index = index;
		children[index] = child;
		numChildren++;
	}

	void remove_child(Oct3Node* child) {
		child->parent = nullptr;
		children[child->index] = nullptr;
		numChildren--;
		child->index = -1;
	}

	void remove_child(int index) {
		if (index >= 0 && index < children.size()) {
			children[index]->parent = nullptr;
			children[index]->index = -1;
			children[index] = nullptr;
			numChildren--;
		}
	}

	bool containsPoint(float x, float y, float z) const {
		if (pos.empty()) {
			return x >= topLeftFront[0] && x <= bottomRightBack[0] &&
				y >= topLeftFront[1] && y <= bottomRightBack[1] &&
				z >= topLeftFront[2] && z <= bottomRightBack[2];
		}
		else {
			return pos[0] == x && pos[1] == y && pos[2] == z;
		}
	}

	bool is_leaf() const {
		return !pos.empty();
	}

	bool is_intermediate() const {
		return pos.empty();
	}

	float getVolume() const {
		return volume;
	}

	void print() {
		std::cout << "Node ------------------" << std::endl;
		if (pos.empty()) {
			std::cout << "  topLeftFront: (" << topLeftFront[0] << ", " << topLeftFront[1] << ", " << topLeftFront[2] << ") " << std::endl;
			std::cout << "  bottomRightBack: (" << bottomRightBack[0] << ", " << bottomRightBack[1] << ", " << bottomRightBack[2] << ") " << std::endl;
			std::cout << " num children: " << numChildren << std::endl;
		}
		else {
			std::cout << "pos: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ") " << std::endl;
		}
		std::cout << "parent: ";

		if (parent != nullptr) {
			if (parent->pos.empty()) {
				std::cout << std::endl;
				std::cout << "  topLeftFront: (" << parent->topLeftFront[0] << ", " << parent->topLeftFront[1] << ", " << parent->topLeftFront[2] << ") " << std::endl;
				std::cout << "  bottomRightBack: (" << parent->bottomRightBack[0] << ", " << parent->bottomRightBack[1] << ", " << parent->bottomRightBack[2] << ") " << std::endl;
				std::cout << "  num children: " << parent->numChildren << std::endl;
				for (int i = 0; i < parent->children.size(); i++) {
					if (parent->children[i] == this) {
						std::cout << "  child pos: " << i <<  std::endl;
						break;
					}
				}
			}
			else {
				std::cout << "(" << parent->pos[0] << ", " << parent->pos[1] << ", " << parent->pos[2] << ") " << std::endl;
			}
		}

		std::cout << "------------------" << std::endl;
		
		for(Oct3Node* child : children) {
			if (child != nullptr)
				child->print();
		}
	}
};

class OctTree {
private:
	Oct3Node* root;

public:
	OctTree(float topLeftFrontX, float topLeftFrontY, float topLeftFrontZ, float bottomRightBackX, float bottomRightBackY, float bottomRightBackZ) {
		root =  new Oct3Node(topLeftFrontX, topLeftFrontY, topLeftFrontZ, bottomRightBackX, bottomRightBackY, bottomRightBackZ, nullptr);
	}

	~OctTree() {
		deallocate(root);
	}

	void insert(float x, float y, float z) {
		insert_root_helper(new Oct3Node(x, y, z, nullptr));
	}

	void insert(std::vector<float>& point) {
		insert_root_helper(new Oct3Node(point[0], point[1], point[2], nullptr));
	}

	void insert(Oct3Node* node) {
		insert_root_helper(node);
	}

	void remove(float x, float y, float z) {
		std::vector<float> pos = { x, y, z };
		Oct3Node* node = this->search(&pos);
		if (node == nullptr) {
			std::cout << "Point not found in tree" << std::endl;
			return;
		}
		removeNode(node);
	}

	void remove(std::vector<float>& point) {
		Oct3Node* node = this->search(&point);
		if (node == nullptr) {
			std::cout << "Point not found in tree" << std::endl;
			return;
		}
		removeNode(node);
	}

	void remove(Oct3Node* node) {
		if (node == nullptr) {
			std::cout << "Point not found in tree" << std::endl;
			return;
		}
		removeNode(node);
	}

	void printTree() {
		root->print();
	}

	Oct3Node* search(Oct3Node* node) {
		return this->search(node, root);
	}

	Oct3Node* search(std::vector<float> *p) {
		Oct3Node node(p->at(0), p->at(1), p->at(2), nullptr);

		return this->search(&node, root);
	}

	std::vector<Oct3Node*> getCollisions(Oct3Node* node) {
		std::vector<Oct3Node*> nodes;
		if (node == nullptr) {
			return nodes;
		}

		Oct3Node* parent = node;

		while (node->volume >= parent->volume) {
			parent = parent->parent;
		}

		this->getAllIntersectingChildren(parent, nodes, node);

		return nodes;
	}

	void moveNode(Oct3Node* node) {
		if (node->parent->containsPoint(node->pos[0], node->pos[1], node->pos[2])) {
			int i = getIndex(node->parent, node);
			if(i != node->index) { 
				node->parent->remove_child(node);
				insertNode(node->parent, node);
			}
		}
		else {
			Oct3Node* parent = node->parent;
			Oct3Node* child = node;
			if (parent == nullptr) { throw std::runtime_error("Parent of node is nullptr"); }
			parent->remove_child(node);

			while (!parent->containsPoint(node->pos[0], node->pos[1], node->pos[2]) || parent != nullptr) {
				if (parent->numChildren == 0) {
					(parent->parent)->remove_child(parent);
					child = parent;
					parent = parent->parent;
					delete child;
					child = nullptr;
				}
				else {
					child = parent;
					parent = parent->parent;
				}
			}

			if (parent == nullptr) { 
				if (child == nullptr) {
					int multiplier = getHighestBase(max(abs(node->pos[0]), abs(node->pos[1]), abs(node->pos[2])))*100;
					this->root = new Oct3Node(-multiplier, -multiplier, -multiplier, multiplier, multiplier, multiplier, nullptr);
					insertNode(root, node);
				}
				else {
					insert_root_helper(node);
				}
			}
			else {
				insertNode(parent, node);
			}
		}
	}

	void draw() {
		int z = (int)(root->bottomRightBack[2] - root->topLeftFront[2]);
		int y = (int)(root->bottomRightBack[1] - root->topLeftFront[1]);
		int x = (int)(root->bottomRightBack[0] - root->topLeftFront[0]);

		std::vector<std::vector<std::vector<char>>> board(z, 
			std::vector<std::vector<char>>(y, 
				std::vector<char>(x, ' ')));

		// draw yx plane
		drawHelper(root, &board, root->topLeftFront[0], root->topLeftFront[1], root->topLeftFront[2]);
		std::vector<std::vector<char>> board2(y, std::vector<char>(x, ' '));

		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				for (int k = 0; k < board[i][j].size(); k++) {
					if (board[i][j][k] == 'X') {
						board2[j][k] = 'X';
					}
					else if (board[i][j][k] == '-') {
						if (board2[j][k] != 'X') {
							board2[j][k] = '-';
						}
					}
				}
			}
		}

		for (int i = 0; i < board2.size(); i++) {
			for (int j = 0; j < board2[i].size(); j++) {
				std::cout << board2[i][j];
			}
			std::cout << std::endl;
		}

	}

private:

	void drawHelper(Oct3Node* node, std::vector<std::vector<std::vector<char>>>* board, int tx, int ty, int tz) {
		if (node == nullptr) {
			return;
		}

		if (node->is_intermediate()) {
			int x, y, z;

			//x
			for (int k : {node->topLeftFront[2] - tz, node->bottomRightBack[2] - tz -1, }) {
				for (int j : {node->topLeftFront[1] - ty, node->bottomRightBack[1] - ty -1, }) {
					for (int i = node->topLeftFront[0]; i < node->bottomRightBack[0]; i++) {
						x = i - tx;
						(*board)[k][j][x] = '-';
					}
				}
			}

			//y
			for (int k : {node->topLeftFront[2] - tz, node->bottomRightBack[2] - tz -1, }) {
				for (int j : {node->topLeftFront[0] - tx, node->bottomRightBack[0] - tx -1, }) {
					for (int i = node->topLeftFront[1]; i < node->bottomRightBack[1]; i++) {
						y = i - ty;
						(*board)[k][y][j] = '-';
					}
				}
			}

			//z
			for (int k : {node->topLeftFront[0] - tx, node->bottomRightBack[0] - tx - 1, }) {
				for (int j : {node->topLeftFront[1] - ty, node->bottomRightBack[1] - ty - 1, }) {
					for (int i = node->topLeftFront[2]; i < node->bottomRightBack[2]; i++) {
						z = i - tz;
						(*board)[z][j][k] = '-';
					}
				}
			}
			
		}

		if (node->is_leaf()) {
			(*board)[(int)node->pos[2] - tz][(int)node->pos[1] - ty][(int)node->pos[0] - tx] = 'X';
		}

		for (Oct3Node* child : node->children) {
			drawHelper(child, board, tx, ty, tz);
		}
	}

	void insert_root_helper(Oct3Node* node) {
		if (root->containsPoint(node->pos[0], node->pos[1], node->pos[2])) {
			insertNode(root, node);
		}
		else {
			int multiplier = getHighestBase(max(abs(node->pos[0]), abs(node->pos[1]), abs(node->pos[2]))) * 100;
			Oct3Node* newRoot = new Oct3Node(-multiplier, -multiplier, -multiplier, multiplier, multiplier, multiplier, nullptr);
			root->parent = newRoot;

			newRoot->add_child(root, getIntermediateNodeIndex(newRoot, root));
			root = newRoot;
			resizeTree(root);
			insertNode(root, node);
		}
	}

	void deallocate(Oct3Node* n) {
		std::vector<Oct3Node*>* nodesToDelete = &n->children;
		for (Oct3Node* child : *nodesToDelete) {
			if(child != nullptr)
				deallocate(child);
		}
		delete n;
	}

	void resizeTree(Oct3Node* node) {
		if (node->is_leaf()) {
			moveNode(node);
		}

		int index;
		float midX = (node->topLeftFront[0] + node->bottomRightBack[0]) / 2;
		float midY = (node->topLeftFront[1] + node->bottomRightBack[1]) / 2;
		float midZ = (node->topLeftFront[2] + node->bottomRightBack[2]) / 2;

		std::vector<Oct3Node*> copyChildren = node->children;

		for (Oct3Node* child : node->children) {
			if (child != nullptr){
				if (child->is_intermediate()) {
					index = getIntermediateNodeIndex(root, node);
					copyChildren[node->index] = nullptr;
					copyChildren[index] = node;

					switch (index)
					{
					case OCT3_TOP_LEFT_FRONT:
						node->topLeftFront = { node->topLeftFront[0], node->topLeftFront[1], node->topLeftFront[2] };
						node->bottomRightBack = { midX, midY, midZ };
						break;
					case OCT3_TOP_RIGHT_FRONT:
						node->topLeftFront = { node->topLeftFront[0], node->topLeftFront[1], midZ };
						node->bottomRightBack = { midX, midY, node->bottomRightBack[2] };
						break;
					case OCT3_BOTTOM_LEFT_FRONT:
						node->topLeftFront = { node->topLeftFront[0], midY, node->topLeftFront[2] };
						node->bottomRightBack = { midX, node->bottomRightBack[1], midZ };
						break;
					case OCT3_BOTTOM_RIGHT_FRONT:
						node->topLeftFront = { midX, midY, node->topLeftFront[2] };
						node->bottomRightBack = { node->bottomRightBack[0], node->bottomRightBack[1], midZ };
						break;
					case OCT3_TOP_LEFT_BACK:
						node->topLeftFront = { node->topLeftFront[0], node->topLeftFront[1], midZ };
						node->bottomRightBack = { midX, midY, node->bottomRightBack[2] };
						break;
					case OCT3_TOP_RIGHT_BACK:
						node->topLeftFront = { midX, node->topLeftFront[1], midZ };
						node->bottomRightBack = { node->bottomRightBack[0], midY, node->bottomRightBack[2] };
						break;
					case OCT3_BOTTOM_LEFT_BACK:
						node->topLeftFront = { node->topLeftFront[0], midY, midZ };
						node->bottomRightBack = { midX, node->bottomRightBack[1], node->bottomRightBack[2] };
						break;
					case OCT3_BOTTOM_RIGHT_BACK:
						node->topLeftFront = { midX, midY, midZ };
						node->bottomRightBack = { node->bottomRightBack[0], node->bottomRightBack[1], node->bottomRightBack[2] };
						break;
					default:
						break;
					}
				}
				resizeTree(child);
				node->children = copyChildren;
			}
		}
	}

	int getIntermediateNodeIndex(Oct3Node* base, Oct3Node* node){
		int index = 0;
		float midX = (base->topLeftFront[0] + base->bottomRightBack[0]) / 2;
		float midY = (base->topLeftFront[1] + base->bottomRightBack[1]) / 2;
		float midZ = (base->topLeftFront[2] + base->bottomRightBack[2]) / 2;

		float nodeX = (node->topLeftFront[0] + node->bottomRightBack[0]) / 2;
		float nodeY = (node->topLeftFront[1] + node->bottomRightBack[1]) / 2;
		float nodeZ = (node->topLeftFront[2] + node->bottomRightBack[2]) / 2;


		if (nodeX < midX) {
			if (nodeY < midY) {
				if (nodeZ < midZ) {
					index = OCT3_TOP_LEFT_FRONT;
				}
				else {
					index = OCT3_TOP_LEFT_BACK;
				}
			}
			else {
				if (nodeZ < midZ) {
					index = OCT3_BOTTOM_LEFT_FRONT;
				}
				else {
					index = OCT3_BOTTOM_LEFT_BACK;
				}
			}
		}
		else {
			if (nodeY < midY) {
				if (nodeZ < midZ) {
					index = OCT3_TOP_RIGHT_FRONT;
				}
				else {
					index = OCT3_TOP_RIGHT_BACK;
				}
			}
			else {
				if (nodeZ < midZ) {
					index = OCT3_BOTTOM_RIGHT_FRONT;
				}
				else {
					index = OCT3_BOTTOM_RIGHT_BACK;
				}
			}
		}
		return index;
	}

	float max(int a, int b, int c) {
		if (a > b) {
			return a > c ? a : c;
		}
		else {
			return b > c ? b : c;
		}
	}

	int getHighestBase(float num) {
		int res = 0;
		num = abs(num);
		while (num > 1) {
			res++;
			num /= 10;
		}
		return pow(10, res);
	}

	int getIndex(Oct3Node* treeNode, Oct3Node* node) {
		int index = 0;
		float midX = (treeNode->topLeftFront[0] + treeNode->bottomRightBack[0]) / 2;
		float midY = (treeNode->topLeftFront[1] + treeNode->bottomRightBack[1]) / 2;
		float midZ = (treeNode->topLeftFront[2] + treeNode->bottomRightBack[2]) / 2;
		if (node->pos[0] < midX) {
			if (node->pos[1] < midY) {
				if (node->pos[2] < midZ) {
					index = OCT3_TOP_LEFT_FRONT;
				}
				else {
					index = OCT3_TOP_LEFT_BACK;
				}
			}
			else {
				if (node->pos[2] < midZ) {
					index = OCT3_BOTTOM_LEFT_FRONT;
				}
				else {
					index = OCT3_BOTTOM_LEFT_BACK;
				}
			}
		}
		else {
			if (node->pos[1] < midY) {
				if (node->pos[2] < midZ) {
					index = OCT3_TOP_RIGHT_FRONT;
				}
				else {
					index = OCT3_TOP_RIGHT_BACK;
				}
			}
			else {
				if (node->pos[2] < midZ) {
					index = OCT3_BOTTOM_RIGHT_FRONT;
				}
				else {
					index = OCT3_BOTTOM_RIGHT_BACK;
				}
			}
		}
		return index;
	}

	void removeNode(Oct3Node* node) {
		if (node == nullptr) {
			return;
		}
		Oct3Node* parent = node->parent;
		while (parent != nullptr) {
			parent->remove_child(node);
			delete node;
			if (parent->numChildren > 0) {
				break;
			}
			node = parent;
			parent = node->parent;
		}
	}


	void insertNode(Oct3Node* treeNode, Oct3Node* node) {
		int index = getIndex(treeNode, node);

		if (treeNode->children[index] == nullptr) {
			treeNode->add_child(node, index);
		} else {
			if (!treeNode->children[index]->pos.empty()) {
				Oct3Node* existingNode = treeNode->children.at(index);
				treeNode->remove_child(index);
				float midX = (treeNode->topLeftFront[0] + treeNode->bottomRightBack[0]) / 2;
				float midY = (treeNode->topLeftFront[1] + treeNode->bottomRightBack[1]) / 2;
				float midZ = (treeNode->topLeftFront[2] + treeNode->bottomRightBack[2]) / 2;
				switch (index)
				{
				case OCT3_TOP_LEFT_FRONT:
					treeNode->add_child(new Oct3Node(treeNode->topLeftFront[0], treeNode->topLeftFront[1], treeNode->topLeftFront[2], midX, midY, midZ, treeNode), index);
					break;
				case OCT3_TOP_RIGHT_FRONT:
					treeNode->add_child(new Oct3Node(midX, treeNode->topLeftFront[1], treeNode->topLeftFront[2], treeNode->bottomRightBack[0], midY, midZ, treeNode), index);
					break;
				case OCT3_BOTTOM_LEFT_FRONT:
					treeNode->add_child(new Oct3Node(treeNode->topLeftFront[0], midY, treeNode->topLeftFront[2], midX, treeNode->bottomRightBack[1], midZ, treeNode), index);
					break;
				case OCT3_BOTTOM_RIGHT_FRONT:
					treeNode->add_child(new Oct3Node(midX, midY, treeNode->topLeftFront[2], treeNode->bottomRightBack[0], treeNode->bottomRightBack[1], midZ, treeNode), index);
					break;
				case OCT3_TOP_LEFT_BACK:
					treeNode->add_child(new Oct3Node(treeNode->topLeftFront[0], treeNode->topLeftFront[1], midZ, midX, midY, treeNode->bottomRightBack[2], treeNode), index);
					break;
				case OCT3_TOP_RIGHT_BACK:
					treeNode->add_child(new Oct3Node(midX, treeNode->topLeftFront[1], midZ, treeNode->bottomRightBack[0], midY, treeNode->bottomRightBack[2], treeNode), index);
					break;
				case OCT3_BOTTOM_LEFT_BACK:
					treeNode->add_child(new Oct3Node(treeNode->topLeftFront[0], midY, midZ, midX, treeNode->bottomRightBack[1], treeNode->bottomRightBack[2], treeNode), index);
					break;
				case OCT3_BOTTOM_RIGHT_BACK:
					treeNode->add_child(new Oct3Node(midX, midY, midZ, treeNode->bottomRightBack[0], treeNode->bottomRightBack[1], treeNode->bottomRightBack[2], treeNode), index);
					break;
				default:
					break;
				}
				insertNode(treeNode->children[index], existingNode);
			}
			insertNode(treeNode->children[index], node);
		}
	}

	Oct3Node* search(Oct3Node* node, Oct3Node* treeNode) {
		if (treeNode == nullptr) {
			return nullptr;
		}
		if (treeNode->pos == node->pos) {
			return treeNode;
		}else if(!treeNode->pos.empty()) {
			return nullptr;
		}

		int index = getIndex(treeNode, node);
		return search(node, treeNode->children[index]);
	}

	void getAllIntersectingChildren(Oct3Node* node, std::vector<Oct3Node*>& nodes, Oct3Node* skipNode=nullptr) {

		if (node == nullptr) {
			return;
		}
		
		for (Oct3Node* child : node->children) {
			if (!child->pos.empty() && child != skipNode) {
				if (skipNode != nullptr) {
					if (GJK(child->getCollider(), skipNode->getCollider())) {
						nodes.push_back(child);
					}
				}
				else {
					nodes.push_back(child);
				}
			}
			else if(child != nullptr && child->pos.empty()) {
				getAllIntersectingChildren(child, nodes);
			}
		}
	}

};