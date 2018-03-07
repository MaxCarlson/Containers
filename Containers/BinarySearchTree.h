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
		Type * data;
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
	Alloc<Type> typeAl;

public:

	BinarySearchTree() = default;
	
	Type * find(const Type &t) // Needs support for rvalues?
	{
		for (Node *p = tree.root; p; ) 
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
	void postorder(Node *p, int indent = 0)
	{
		if (p)
		{
			if (p->subTree[0]) postorder(p->subTree[0], indent + 4);
			if (p->subTree[1]) postorder(p->subTree[1], indent + 4);
			if (indent)
			{
				std::cout << std::setw(indent) << ' ';
			}
			std::cout << *p->data << "\n ";
		}
	}

	Node * minValueNode(Node *n)
	{
		// Find the left most node from this node
		while (n->subTree[0])
			n = n->subTree[0];
		return n;
	}

	Node* deleteNode(Node * n, const Type &t) // Issue with stack size when tree is not balanced
	{
		if (n == nullptr) 
			return n;

		// Key is farther left in the tree
		if (t < *n->data)
			n->subTree[0] = deleteNode(n->subTree[0], t);

		// Farther right
		else if (t > *n->data)
			n->subTree[1] = deleteNode(n->subTree[1], t);

		// Found Key
		else
		{
			// No left child or no children
			if (n->subTree[0] == nullptr)
			{
				auto* tmp = n->subTree[1];
				typeAl.free(n->data);
				nodeAl.free(n);
				--tree.count;
				return tmp;
			}
			// No right child
			else if (n->subTree[1] == nullptr)
			{
				auto* tmp = n->subTree[0];
				typeAl.free(n->data);
				nodeAl.free(n);
				--tree.count;
				return tmp;
			}

			// Two children

			// Find the In Order Successor, or the smallest node in this nodes
			// right subtree
			Node * small = minValueNode(n->subTree[1]);

			typeAl.free(n->data);
			n->data = small->data;

			n->subTree[1] = deleteNode(n->subTree[1], *small->data);
		}
	}

public:
	void erase(const Type &t) // TODO: Add an iterator delete that returns next iterator
	{
		tree.root = deleteNode(tree.root, t);
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