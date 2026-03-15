#include <vector>
#include <iostream>

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
	int numChildren = 0;

	Oct3Node(float x, float y, float z, Oct3Node* parent) : pos{ x, y, z }, parent(parent) {
		children.resize(8);
	}
	Oct3Node(float topLeftFrontX, float topLeftFrontY, float topLeftFrontZ, float bottomRightBackX, float bottomRightBackY, float bottomRightBackZ, Oct3Node* parent)
		: parent(parent), topLeftFront({ topLeftFrontX, topLeftFrontY, topLeftFrontZ }), bottomRightBack({ bottomRightBackX, bottomRightBackY, bottomRightBackZ })
	{
		children.resize(8);
	}

	void add_child(Oct3Node* child, int index) {
		child->parent = this;
		children[index] = child;
		numChildren++;
	}

	void remove_child(Oct3Node* child) {
		child->parent = nullptr;
		for (int i = 0; i < children.size(); i++) {
			if (children[i] == child) {
				children[i] = nullptr;
				numChildren--;
				return;
			}
		}
	}

	void remove_child(int index) {
		if (index >= 0 && index < children.size()) {
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
		return numChildren == 0;
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
		insertNode(root, new Oct3Node(x, y, z, nullptr));
	}

	void insert(std::vector<float>& point) {
		insertNode(root, new Oct3Node(point[0], point[1], point[2], nullptr));
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

private:
	void deallocate(Oct3Node* n) {
		std::vector<Oct3Node*>* nodesToDelete = &n->children;
		for (Oct3Node* child : *nodesToDelete) {
			if(child != nullptr)
				deallocate(child);
		}
		delete n;
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
		}
		else {
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

};