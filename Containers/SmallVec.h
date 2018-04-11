#pragma once
#include <memory>


#include <new>

// TODO:
template<class Type, int alignedSize, class Allocator = std::allocator<Type>>
class SmallVec
{
public:
	using AlTraits = std::allocator_traits<Allocator>;
	using Storage = typename std::aligned_storage<sizeof(Type), alignof(Type)>::type;

	using size_type = size_t;
	using difference_type = typename AlTraits::difference_type;

	using NodePtr = Type *;

private:
	Allocator alloc;

	Storage data[alignedSize];

	unsigned int useAligned		 : 1;
	unsigned int copyFromAligned : 1;

	size_type MySize = 0;
	size_type MyCapacity = alignedSize;

	NodePtr MyBegin = nullptr;
	NodePtr MyEnd = nullptr;

public:
	SmallVec() : useAligned(true), copyFromAligned(false) {}

private:

	void copyTo(NodePtr first)
	{
		NodePtr oldFirst = MyBegin;
		NodePtr oldEnd = MyEnd;
		if (copyFromAligned)
		{
			oldFirst = reinterpret_cast<NodePtr>(&data[0]);
			oldEnd = reinterpret_cast<NodePtr>(&data[alignedSize]); // TODO: Is this a safe way to do this from aligned storage?
		}
			
		for (oldFirst; oldFirst != oldEnd; ++oldFirst, ++first)
			*first = std::move(*oldFirst);		


		if (copyFromAligned)
			copyFromAligned = false;
		else
			AlTraits::deallocate(alloc, MyBegin, MyCapacity);	
	}
	
	void grow()
	{
		const size_type newCapacity = MyCapacity + (MyCapacity + 4) / 2; // TODO: Revisit Growth Rates

		NodePtr first = alloc.allocate(newCapacity);
		NodePtr last = first + static_cast<difference_type>(newCapacity);

		copyTo(first);

		MyCapacity = newCapacity;
		MyBegin = first;
		MyEnd = last;
	}

public:

	template<class... Args>
	void emplace_back(Args&& ...args)
	{
		if (useAligned)
		{
			//AlTraits::construct(alloc, data + MySize, std::forward<Args>(args)...); // TODO: Figure out if we can use allocator construction here
			new(data + MySize) Type(std::forward<Args>(args)...);

			if (MySize >= alignedSize - 1)
			{	// Don't use aligned storage again after this
				copyFromAligned = true;
				grow();
				useAligned = false;
			}
		}
		else	
			AlTraits::construct(alloc, MyBegin + MySize, std::forward<Args>(args)...);

		++MySize;

		if (MySize > MyCapacity - 1)
			grow();
	}
};