#pragma once
#include <stdlib.h>

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
		free(start); 
	}
};

template<class Type> // TODO: Add comparator and allocator
class BinarySearchTree
{
private:
	struct Node
	{
		Node * subTree[2];
		Type * data;
	};

	struct Table
	{
		Node * root = nullptr;
		// Add comparator
		// Add allocator
		int count = 0;
		size_t genNumber = 0; // Keeps track of tree's state
	};

public:

	Table tree;

	// Allocator al;
	Alloc<Node> al;

	BinarySearchTree() = default;
	
	Type * find(const Type &t) // Needs support for rvalues?
	{
		for (Node *p = tree.root; p; ) // p != nullptr needed ?
		{
			if (t < *p->data)
				p = p->subTree[0];
			else if (t > *p->data)
				p = p->subTree[1];
			else
				return p->data;
		}

		return nullptr; // TODO: Return iterator to end?
	}

	void emplace(Type&& t)
	{
		int dir = 0;
		Node *p, *c; // parent and current node
		for (c = tree.root; c; p = c) 
		{
			if (t < *c->data)
				dir = 0;
		
			else if (t > *c->data)	
				dir = 1;
			
			else
				return; // Found Identical node

			c = c->subTree[dir];
		}
		// Fall through

		c = al.alloc();
		c->data = &t;
		c->subTree[0] = c->subTree[1] = nullptr;

		if (c)
			p->subTree[dir] = c;
		else
			tree.root = c;

		++tree.count;
	}

	void insert(const Type& t)
	{
		emplace(t);
	}

	void insert(Type &&t)
	{
		emplace(std::move(t));
	}

};