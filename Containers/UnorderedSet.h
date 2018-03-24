#pragma once
#include <utility>

template<class Key, 
	class KeyEqual,
	class Allocator>
	struct USetTraits
{

};

template<class Key,
	template<class> MyBase,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>,
	class Allocator = std::allocator<Key>>
	class UnorderedSet : public MyBase<USetTraits<Key, KeyEqual, Allocator>>
{

};