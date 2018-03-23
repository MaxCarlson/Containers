#pragma once
#include "RbTree.h"

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

template<class Key,
	template<class> class MyBase = RedBlackTree,
	class Compare = std::less<Key>,
	class Allocator = std::allocator<Key>>
	class Set : public MyBase<SetTraits<Key, Compare, Allocator>>
{
//	using MyBas = MyBase<SetTraits<Key, Compare, Allocator>>;
};