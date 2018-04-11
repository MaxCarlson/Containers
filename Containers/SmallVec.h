#pragma once
#include <memory>

template<class VectorType>
class VecIterator 
{
public:
	using NodePtr = typename VectorType::NodePtr;
	using reference = typename VectorType::reference;
	using pointer = typename VectorType::pointer;

	VecIterator() = default;
	VecIterator(VectorType *MyVec) : MyVec(MyVec) {}
	VecIterator(VectorType *MyVec, NodePtr ptr) : MyVec(MyVec), ptr(ptr) {}

	reference operator*()
	{
		return ptr;
	}

	pointer operator->()
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	VecIterator& operator++()
	{
		if (ptr <= MyVec->Mylast)
			++ptr;

		return *this;
	}

	VecIterator operator++(int)
	{
		HashIterator tmp = *this;
		++*this;
		return tmp;
	}

	VecIterator& operator--()
	{
		if (ptr > MyVec->MyBegin)
			--ptr;

		return *this;
	}

	VecIterator operator--(int)
	{
		VecIterator tmp = *this;
		--*this;
		return tmp;
	}

	bool operator==(const VecIterator& other) const
	{
		return this->ptr == other.ptr;
	}

	bool operator!=(const VecIterator& other) const
	{
		return !(this->ptr == other.ptr);
	}

private:
	NodePtr ptr;
	VectorType *MyVec;
};


template<class Type, int alignedSize, class Allocator = std::allocator<Type>>
class SmallVec
{
public:
	using MyBase = SmallVec<Type, alignedSize, Allocator>;
	using AlTraits = std::allocator_traits<Allocator>;
	using AlignedStorage = typename std::aligned_storage<sizeof(Type), alignof(Type)>::type;

	using size_type = size_t;
	using difference_type = typename AlTraits::difference_type;
	using pointer = typename AlTraits::pointer;
	using reference = Type&;
	using const_reference = const reference;
	using const_pointer = const pointer;

	using NodePtr = Type *;

	using iterator = VecIterator<MyBase>;

private:
	Allocator alloc;

	AlignedStorage data[alignedSize];

	unsigned int useAligned		 : 1;
	unsigned int copyFromAligned : 1;

	size_type MySize = 0;
	size_type MyCapacity = alignedSize;

	NodePtr MyBegin = nullptr;
	NodePtr MyLast = nullptr;
	NodePtr MyEnd = nullptr;

public:

	// In the case aligned size is zero, don't use aligned memory
	SmallVec() : useAligned(static_cast<bool>(alignedSize)), 
		   copyFromAligned(!static_cast<bool>(alignedSize)) 
	{
		if (useAligned)
		{
			MyBegin = MyLast = reinterpret_cast<NodePtr>(&data[0]);
		}
	}

	iterator begin() { return iterator{ this, MyBegin }; }
	iterator end() { return iterator{ this, MyLast }; }


private:

	void copyTo(NodePtr first) // TODO: More of a reallocate function, split it up so we can use the copy mechanics elsewhere!
	{
		NodePtr oldFirst = MyBegin;
		NodePtr oldEnd = MyEnd;
		if (copyFromAligned)
		{
			oldFirst = reinterpret_cast<NodePtr>(&data[0]);
			oldEnd = reinterpret_cast<NodePtr>(&data[alignedSize]); // TODO: Is this a safe way to do this from aligned storage?
		}
			
		for (oldFirst; oldFirst != oldEnd; ++oldFirst, ++first)
		{
			*first = std::move(*oldFirst);
			AlTraits::destroy(alloc, oldFirst);
		}

		if (copyFromAligned)
			copyFromAligned = false;
		else
			AlTraits::deallocate(alloc, MyBegin, MyCapacity);	
	}
	
	void grow()
	{
		const size_type newCapacity = MyCapacity + (MyCapacity + 1) / 2; // TODO: Revisit Growth Rates

		NodePtr first = alloc.allocate(newCapacity);
		NodePtr last = first + static_cast<difference_type>(newCapacity);

		copyTo(first);

		// If we were using AlignedStorage previously (or not!) this is where
		// MyBegin and MyEnd get switched over out of the AlignedStorage array
		MyCapacity = newCapacity;
		MyBegin = first;
		MyLast = MyBegin + MySize;
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
			{	// Don't use aligned storage again after this threshold is passed!
				copyFromAligned = true;
				grow();
				useAligned = false;
			}
		}
		else	
			AlTraits::construct(alloc, MyBegin + MySize, std::forward<Args>(args)...);

		++MySize;
		++MyLast; // TODO: Would this just be better to calculate this on the spot for the end() func?

		if (MySize > MyCapacity - 1)
			grow();
	}

	void pop_back()
	{	// Don't call with an empty vector!
		if (useAligned)
			reinterpret_cast<NodePtr>(&data[MySize - 1])->~Type(); // TODO: Revisit these
		
		else
			AlTraits::destroy(alloc, MyBegin + (MySize - 1));
		
		--MySize;
		--MyLast; // TODO: Would this just be better to calculate this on the spot for the end() func?
	}

	bool empty() const noexcept
	{
		return static_cast<bool>(MySize);
	}

	size_type size() const noexcept
	{
		return MySize;
	}

	size_type capacity() const noexcept
	{
		return MyCapacity;
	}

	void clear() noexcept // TODO:
	{
		NodePtr first = MyBegin;
		NodePtr last = MyBegin + (MySize + 1);

		if (useAligned)
		{
			first = reinterpret_cast<NodePtr>(&data[0]);
			last  = reinterpret_cast<NodePtr>(&data[MySize]); // TODO: Is this a safe way to do this from aligned storage?
		}

		for (first; first != last; ++first)
			AlTraits::destroy(alloc, first);

		MySize = 0;
	}
};