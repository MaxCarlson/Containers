#pragma once
#include <utility>
#include "OpenAddressLT.h"

template<class Key,
	class Hash,
	class KeyEqual,
	class Allocator>
	struct USetTraits
{
	using key_type		 = Key;
	using value_type	 = Key;
	using hasher		 = Hash;
	using key_equal		 = KeyEqual;
	using allocator_type = Allocator;

	using node_type = key_type;

	struct node_equal
	{
		bool operator()(const node_type& left, const node_type& right) const
		{
			return comp(left, right);
		}

		key_equal comp;
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
	template<class, bool> class MyBase = OpenAddLinear,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>,
	class Allocator = std::allocator<Key>>
	class UnorderedSet : public MyBase<USetTraits<Key, Hash, KeyEqual, Allocator>, false>
{

};