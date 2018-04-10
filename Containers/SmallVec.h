#pragma once
#include <memory>

// TODO:
template<class Type, int size, class Allocator = std::allocator<Type>>
class SmallVec
{

	using AlTraits = std::allocator_traits<Allocator>;
	using Storage = typename std::aligned_storage<sizeof(Type), alignof(Type)>::type;



private:
	Storage data[size];
};