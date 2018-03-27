#pragma once
#include <utility>
#include "OpenAddressLT.h"

template<class Key, class Value,
	class Hash,
	class KeyEqual,
	class Allocator>
	struct UMapTraits
{
	using key_type = Key;
	using value_type = Value;
	using hasher = Hash;
	using key_equal = KeyEqual;
	using allocator_type = Allocator;

	using node_type = std::pair<Key, Value>; // const Key

	struct node_equal
	{
		bool operator()(const node_type& left, const node_type& right) const
		{
			return comp(left.first, right.first);
		}

		key_equal comp;
	};

	struct get_key
	{
		const key_type& operator()(const node_type& n) const
		{
			return n.first;
		}
	};
};


template<class Key, class Value,
	template<class, bool> class MyBase = OpenAddressLT,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>,
	class Allocator = std::allocator<std::pair<Key, Value>>> // const Key
	class UnorderedMap : public MyBase<UMapTraits<Key, Value, Hash, KeyEqual, Allocator>, false>
{

};