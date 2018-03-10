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
		Node * parent;
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
		n->data = t;   // The constructor is being called before the move yes? Should this be allocated on the heap?
		n->subTree[0] = n->subTree[1] = n->parent = nullptr;
		return n;
	}

	void emplace(Type&& t)
	{
		if (tree.root == nullptr)
			tree.root = createNode(std::move(t));

		else
		{
			int dir = 0;
			Node *c = tree.root; 
			for (;;)
			{
				dir = (c->data < t);

				if (c->data == t) // Do not overwrite data here
					return;

				else if (c->subTree[dir] == nullptr) // Break when we encounter a leaf node next
					break;

				c = c->subTree[dir];
			}

			c->subTree[dir] = createNode(std::move(t));
			c->subTree[dir]->parent = c;
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

	void deleteN(const Type& t)
	{
		if (tree.root)
		{

		}
	}

	bool deleteNode(const Type& t)
	{
		if (tree.root)
		{
			Node head;
			Node *c = &head;
			Node *p = nullptr, *found = nullptr;
			int dir = 1;

			c->subTree[1] = tree.root;

			while (c->subTree[dir])
			{
				// Set parent
				p = c;

				// Navigate subtrees
				c = c->subTree[dir];
				dir = c->data <= t;

				// Found node to delete
				// Keep going to find in order successor
				// parent and child
				if (c->data == t)
					found = c; 
			}

			if (found)
			{
				found->data = c->data;
				p->subTree[p->subTree[1] == c] = c->subTree[c->subTree[0] == nullptr];
				nodeAl.free(c);
				--tree.count;
			}

			tree.root = head.subTree[1];
			// TODO: Return next in place iterator for iterator version
			return found;
		}
		return false;
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