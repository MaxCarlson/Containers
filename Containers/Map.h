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

	using node_type = std::pair<const key_type, value_type>;

	struct value_compare
	{
		bool operator()(const node_type& left, const node_type& right) const
		{
			return comp(left.first, right.first);
		}

		key_compare comp;
	};

	struct get_key
	{
		const key_type& operator()(const node_type& n) const
		{
			return n.first;
		}
	};
};

template<class Key, class Type,
	template<class> class MyBase = RedBlackTree,
	class Compare = std::less<Key>,
	class Allocator = std::allocator<std::pair<const Key, Type>>> 
	class Map : public MyBase<MapTraits<Key, Type, Compare, Allocator>>
{
	//using MyBase = MyBase<MapTraits<Key, Type, Compare, Allocator>>;
};