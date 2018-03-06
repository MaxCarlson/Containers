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

	Table tree;

	// Allocator al;
	Alloc<Node> nodeAl;
	Alloc<Type> typeAl;

public:

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
		Node *p = nullptr, *c; // parent and current node
		for (c = tree.root; c;) 
		{
			if (t < *c->data)
				dir = 0;
		
			else if (t > *c->data)	
				dir = 1;
			
			else
				return; // Found Identical node

			p = c;
			c = c->subTree[dir];
		}
		// Fall through

		c = nodeAl.alloc();
		c->data = typeAl.alloc(); 

		*c->data = *std::move(&t);
		c->subTree[0] = c->subTree[1] = nullptr;

		if (p)
			p->subTree[dir] = c;
		else
			tree.root = c;

		++tree.count;
	}

	void insert(const Type& t)
	{
		emplace(std::move(t));
	}

	void insert(Type &&t)
	{
		emplace(std::move(t));
	}

private:
	void flatten()
	{
		Node *c = tree.root, *p;

		while (c)
		{
			if (!c->subTree[1]) // If no right child
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

	void balance()
	{
		flatten();
	}
};