#pragma once
#include "RedBlackTree.h"

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

// Map is just a wrapper for differerent ordered class templates
// Default is red-black tree at the moment.
// 
// TODO: Write copy and move constructors that are compatible
// between base template classes
template<class Key, class Type,
	template<class> class MyBase = RedBlackTree,
	class Compare = std::less<Key>,
	class Allocator = std::allocator<std::pair<const Key, Type>>> 
	class Map : public MyBase<MapTraits<Key, Type, Compare, Allocator>>
{

};