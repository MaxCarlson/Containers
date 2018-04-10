#pragma once
#include <memory>

// TODO:
template<class Type, int size, class Allocator = std::allocator<Type>>
class SmallVec
{
public:
	using AlTraits = std::allocator_traits<Allocator>;
	using Storage = typename std::aligned_storage<sizeof(Type), alignof(Type)>::type;
	using size_type = size_t;

	using NodePtr = Type *;

private:
	Allocator alloc;

	Storage data[size];

	size_type MySize;

	NodePtr MyBegin;
	NodePtr MyEnd;
	
	void grow()
	{
		const size_type newSize = MySize + (MySize + 4) / 2; // TODO: Revisit this
	}
};