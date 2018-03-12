#pragma once
#include <functional>






template<class Type, class Compare = std::less<Type>, class Allocator = std::allocator<Type>>
class RedBlackTree
{
	using RbTree = RedBlackTree<Type, Compare, Allocator>;

	Compare compare;
	Allocator alloc;

//public:

	enum Color { RED, BLACK };
	enum Direction { LEFT, RIGHT };

	struct Node
	{
		Node* parent;
		Node* subtree[2];

		char color;
		Type data;
	};

	Node * root;

	// TODO: Add support for <key, type> containers

public:
	RedBlackTree()
	{

	}

private: 

public:
	// Pass this directions only
	template<Direction dir>
	Node *rotate(Node *root)
	{
		Node* newRoot = root->subtree[!dir];

		// Switch old root and new root
		root->subtree[!dir] = newRoot->subtree[dir];
		newRoot->subtree[dir] = root;

		// Set new colors
		root->color = RED;
		newRoot->color = BLACK;

		return newRoot;
	}

	
	template<Direction dir>
	Node *doubleRotation(Node *root)
	{
		// Rotate opposite direction on opposite of dir's subtree
		root->subtree[!dir] = rotate<!dir>(root->subtree[!dir]);

		return rotate<dir>(root);
	}

	bool isRed(Node *n)
	{
		return (n && n->color == RED);
	}

	int testTree(Node *root)
	{
		if (!root)
			return 1;

		Node* left = root->subtree[LEFT];
		Node* right = root->subtree[RIGHT];

		// Red node has a red child
		if (isRed(root))
			if (isRed(left) || isRed(right))
			{
				throw std::runtime_error("Red Violation!");
				return 0;
			}

		int lHeight = testTree(left);
		int rHeight = testTree(right);

		if ((left && left->data >= root->data) || (right && right->data <= root->data)) // This does not use Compare!!!
		{
			throw std::runtime_error("Invalid Binary Tree!");
			return 0;
		}

		if (lHeight && rHeight && lHeight != rHeight)
		{
			throw std::runtime_error("Black Height mis-match!");
			return 0;
		}

		// Only cound black links
		if (lHeight && rHeight)
			return isRed(root) ? lHeight : lHeight + 1;

		return 0;
	}
};

// Notes: 
// Root is black
// Node that's colored Red cannot have a child Colored Red ~ red violation if not true
// Number of black nodes along any path in the tree must be identical ~ black height of the tree ~ black violation if not  true

// Height cannot be shorter than log(N + 1) and cannot be taller than 2 * Log(N + 1)


// Rotations:
// A single rotation sets the old root to Red and the new root to black
// A double rotation

// Double rotation
// - rotate once in !direction of initial dir on the !dir subtree
// - rotate once on the root in rotation direction
