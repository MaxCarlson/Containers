#pragma once
#include "RedBlackTree.h"

template<class Key, 
	class Compare,
	class Allocator>
	struct SetTraits
{
	using key_type = Key;
	using value_type = Key;
	using key_compare = Compare;
	using allocator_type = Allocator;

	using node_type = const key_type; 

	struct value_compare
	{
		bool operator()(const node_type& left, const node_type& right) const
		{
			return comp(left, right);
		}

		key_compare comp;
	};

	struct get_key
	{
		const key_type& operator()(const node_type& n) const
		{
			return n;
		}
	};

};

// Set is just a wrapper for differerent ordered class templates
// Default is red-black tree at the moment.
// 
// TODO: Write copy and move constructors that are compatible
// between base template classes
template<class Key,
	template<class> class MyBase = RedBlackTree,
	class Compare = std::less<Key>,
	class Allocator = std::allocator<Key>>
	class Set : public MyBase<SetTraits<Key, Compare, Allocator>>
{
public:

	template<class, template<class> class, class, class>
	friend class Set;

	//using UnderlyingT = MyBase<SetTraits<Key, Compare, Allocator>>;


};