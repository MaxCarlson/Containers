#pragma once
#include "RbTree.h"


template<class Key, class Type,
	class Compare,
	class Allocator>
	struct MapTraits
{
	using key_type = Key;
	using value_type = Type;
	using key_compare = Compare;
	using allocator_type = Allocator;

	using node_type = std::pair<key_type, value_type>;

	struct value_compare
	{
		/*
		value_compare(key_compare comp)
			: comp(comp)
		{
		}
		*/
		
		bool operator()(const node_type& left, const node_type& right)
		{
			return comp(left.first, right.first);
		}

		key_compare comp;
	};
};

template<class Key, class Type, 
	class Compare = std::less<Key>, 
	class Allocator = std::allocator<std::pair<const Key, Type>>, 
	class MyBase = RedBlackTree<MapTraits<Key, Type, Compare, Allocator>>> // This is awkward having this as last param in template
class Map : public MyBase														 // Also awkward having to fill in already known types and pair vs non-pair ~ TODO: Fix
{
	
};