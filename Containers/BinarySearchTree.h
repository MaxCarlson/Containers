#pragma once
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <intrin.h>
#include <utility>
#include <tuple>

template<class Type>
struct Alloc
{
	Type * alloc()
	{
		Type * t = new Type;
		return t;
	}

	void free(Type * start)
	{
		std::free(start); 
	}
};

template<class Type> // TODO: Add comparator and allocator
class BinarySearchTree
{
private:
	struct Node
	{
		Node * subTree[2];
		Type data; 
	};

	struct Table
	{
		Node * root = nullptr;
		// Add comparator
		// Add allocator
		size_t count = 0;
		size_t genNumber = 0; // Keeps track of tree's state
	};

	Table tree;

	// Allocator al;
	Alloc<Node> nodeAl;

public:

	BinarySearchTree() = default;
	
	Type * find(const Type &t) 
	{
		Node *p = tree.root;
		while (p)
		{
			if (p->data == t)
				return &p->data;
			else
			{
				int dir = p->data < t;
				p = p->subTree[dir];
			}
		}

		return nullptr;
	}

	Node * createNode(Type &&t)
	{
		Node * n = nodeAl.alloc();
		n->data = std::move(t);   // The constructor is being called before the move yes?
		n->subTree[0] = n->subTree[1] = nullptr;
		return n;
	}

	void emplace(Type&& t)
	{
		if (tree.root == nullptr)
			tree.root = createNode(std::move(t));

		else
		{
			int dir = 0;
			Node *c = tree.root; // parent and current node
			for (;;)
			{
				dir = (c->data < t);

				if (c->data == t) // Identical data, break out of func
					return;

				else if (c->subTree[dir] == nullptr) // Break when we encounter a leaf node next
					break;

				c = c->subTree[dir];
			}

			c->subTree[dir] = createNode(std::move(t));
		}

		++tree.count;
	}

	void insert(Type t)
	{
		emplace(std::move(t));
	}

	void insert(Type &&t)
	{
		emplace(std::move(t));
	}

	void printTree()
	{
		postorder(tree.root);
	}
private:

	bool deleteNode(const Type& t)
	{
		int dir;
		Node *p = nullptr;
		Node *c = tree.root;

		// Find the parent and the node == t
		for(;;)
		{
			if (!c)
				return false;

			else if (c->data == t)
				break;

			dir = c->data < t;
			p = c;
			c = c->subTree[dir];
		}
		
		// Both subtrees have nodes
		if (c->subTree[0] && c->subTree[1])
		{
			p = c;

			Node *successor = c->subTree[1];

			// Find the in order successor
			while (successor->subTree[0])
			{
				p = successor;
				successor = successor->subTree[0];
			}

			// Copy data from successor into the node we're moving
			// to the deleted nodes spot
			c->data = successor->data;

			// Connect parent to correct child
			p->subTree[p->subTree[1] == successor] = successor->subTree[1];

			nodeAl.free(successor);
		}
		else
		{
			dir = c->subTree[0] == nullptr;

			// Handle root deletion
			if (!p)
				tree.root = c->subTree[dir];

			// Handle 
			else
				p->subTree[p->subTree[1] == c] = c->subTree[dir];

			nodeAl.free(c);
		}

		--tree.count;
		return true;
		// TODO: Return next in place iterator for iterator version
	}

public:
	void erase(const Type &t) // TODO: Add an iterator delete that returns next iterator
	{
		deleteNode(t);
	}

	struct Iterator 
	{
		Type *p;
	private:
		int height = 0;
		bool left = true;
	};

	Iterator& operator++()
	{

	}

private:

	Iterator MyBegin;
	Iterator MyEnd;

	size_t pop_msb(size_t b)
	{
		unsigned long idx;
		_BitScanReverse64(&idx, b);
		return b ^ (1 << idx);
	}

	int calculateLeaves()
	{
		return pop_msb(tree.count + 1);
	}

	void flatten()
	{
		Node *c = tree.root, *p = tree.root; // There's an issue with infinite loop here at the moment

		while (c)
		{
			if (c->subTree[1] == nullptr) // If no right child
			{
				p = c;
				c = c->subTree[0]; // Step parent and child down and to the left
			}
			else // Rotate left at parent
			{
				auto* r = c->subTree[1];
				
				c->subTree[1] = r->subTree[0];   // Childs right node becomes previous rights nodes left node
				r->subTree[0] = c;			     // previous childs right tree left node becomes child
				c = r;						
				p->subTree[0] = r; // parent nodes left subtree becomes previous childs right node
			}
		}
	}

	void compressFlatTree(int count)
	{
		while (--count)
		{
			Node *red = tree.root->subTree[0];
			Node *black = red->subTree[0];

			tree.root->subTree[0] = black;
			red->subTree[0] = black->subTree[1];
			black->subTree[1] = red;
			tree.root = black;
		}
	}
public:
	void balance()
	{
		flatten();

		int leaves = calculateLeaves();
		int vine = static_cast<int>(tree.count) - leaves;
		int height = 1 + (leaves > 0);

		while (vine > 0)
		{
			compressFlatTree(vine /= 2);
			++height;
		}
	}
};