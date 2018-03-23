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
	template<class> class MyBase = RedBlackTree> 
	class Map : public MyBase<MapTraits<Key, Type, Compare, Allocator>>
{

};