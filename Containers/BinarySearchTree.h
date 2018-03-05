#pragma once
#include <stdlib.h>

template<class Type>
class Alloc
{
	Type * alloc()
	{
		return malloc(sizeof(type));
	}
};

template<class Type, class Allocator> // TODO: Add comparator and allocator
class BinarySearchTree
{
private:
	struct Node
	{
		Node * subTrees[2];
		Type * data;
	};

	struct Table
	{
		Node * root;
		// Add comparator
		// Add allocator
		int count;
		size_t genNumber; // Keeps track of tree's state
	};

	Table table;

public:

	BinarySearchTree()
	{

	}
};