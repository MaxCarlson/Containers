#pragma once
#include <iostream>
//#include <iomanip>
//#include <stdlib.h>
//#include <intrin.h> // Not Working with Intel Compiler?
#include <utility>
#include <string>
#include <typeinfo>

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

	struct Iterator
	{
		Iterator() = default;
		Iterator(Node *n, BinarySearchTree<Type> *t) : it(n), tree(t)
		{
			data = &n->data;
		}

		bool operator!=(const Iterator& i) const
		{
			return data != i.data; 
		}

		bool operator==(const Iterator& i) const
		{
			return data == i.data;
		}

		const Type & operator*() 
		{
			return *data;
		}

		Iterator& operator++() // Preincrement
		{
			if (!data) // Don't increment on null node, usually end()
				; 
			else if (it->subTree[1]) // Right node is non empty, find smallest member
			{
				it = smallestOfSubTree(it->subTree[1]);
				data = &it->data; 
			}
			else // Climb up tree looking for first Node with a non-empty right subtree
			{
				Node* n = it;

				bool found = false;
				/*
				while (n->parent)
				{
					n = n->parent;

					if (n->data > it->data)
					{
						it = n;
						data = &it->data;
						found = true;
						break;
					}
				}
				*/

				// While node has a parent and the iterator is equal
				// to its parents right node, traverse up tree
				while ((n = n->parent) && it == n->subTree[1]) 
					it = n;

				if (n)
				{
					it = n;
					data = &it->data;
					found = true;
				}

				// Inelegant way to set Iterator to end
				if (!found)
					*this = tree->Head;
			}
			return *this;
		}

		Iterator operator++(int) // Postincrement
		{
			Iterator i = *this;
			++*this;
			return i;
		}

	protected:
		friend class BinarySearchTree<Type>;
		Node *it;
		Type *data; // Get rid of this and allocate node data sepperatly from nodes?
		BinarySearchTree<Type> * tree; // How to get rid of this? Need it for head currently
	};

	static Node* smallestOfSubTree(Node *start)
	{
		if (!start)
			throw std::runtime_error(std::string("Invaid pointer passed to navigating Iterator in BST<") + typeid(Type).name() + std::string(">!"));

		while (start->subTree[0])
			start = start->subTree[0];

		return start;
	}

private:
	Iterator Head;

public:

	BinarySearchTree()
	{
		// Allocate a head node
		Head = Iterator(nodeAl.alloc(), this);
		Head.data = nullptr;
	}

	Iterator begin()
	{
		return Iterator(leftMost(tree.root), this);
	}
	Iterator end() 
	{ 
		return Head;
	}
	
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

	void insert(const Type& t)
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

	bool deleteNode(const Type& t) noexcept
	{
		if (tree.root)
		{
			Node head;
			Node *c = &head;
			Node *found = nullptr;
			int dir = 1;

			c->subTree[1] = tree.root;
			tree.root->parent = &head;

			while (c->subTree[dir])
			{
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
				dir = c->subTree[0] == nullptr;

				found->data = c->data;

				c->parent->subTree[c->parent->subTree[1] == c] = c->subTree[dir];

				if (c->subTree[dir])
					c->subTree[dir]->parent = c->parent;
				

				nodeAl.free(c);
				--tree.count;
			}

			tree.root = head.subTree[1];

			if (tree.root)
				tree.root->parent = nullptr;


			return found;
		}
		return false;
	}

	// Find the left most node from the subtree
	// of the input node
	Node* leftMost(Node *start)
	{
		if (start == nullptr)
			return Head.it;

		while (start->subTree[0])
			start = start->subTree[0];

		return start;
	}

public:
	bool erase(const Type &t) noexcept // TODO: Add an iterator delete that returns next iterator
	{
		return deleteNode(t);
	}

private:

	/*
	size_t pop_msb(size_t b)
	{
		unsigned long idx;
		//_BitScanReverse64(&idx, b);
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
	*/

};