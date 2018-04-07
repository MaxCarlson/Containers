#pragma once
#include <utility>
#include "RobinHood.h"

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

	using node_type = std::pair<const Key, Value>; 

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
		const key_type& operator()(const node_type& n) const noexcept
		{
			return n.first;
		}
	};
};


template<class Key, class Value,
	template<class, bool> class MyBase = RobinhoodHash,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>,
	class Allocator = std::allocator<std::pair<const Key, Value>>> 
	class UnorderedMap : public MyBase<UMapTraits<Key, Value, Hash, KeyEqual, Allocator>, false>
{
public:

	//using Base = MyBase<UMapTraits<Key, Value, Hash, KeyEqual, Allocator>, false>;
	//using iterator = typename Base::iterator;
	//using const_iterator = typename Base::const_iterator;
};