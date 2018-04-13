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
		return *ptr;
	}

	pointer operator->()
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	VecIterator& operator++()
	{
		if (ptr <= MyVec->MyLast)
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
	const VectorType *MyVec;
};

template<class VectorType>
class ConstVecIterator : public VecIterator<VectorType>
{
public:
	using MyBase = VecIterator<VectorType>;
	using NodePtr = typename VectorType::NodePtr;
	using reference = typename VectorType::const_reference;
	using pointer = typename VectorType::const_pointer;

	ConstVecIterator() = default;
	ConstVecIterator(VectorType *MyVec) : MyBase(MyVec) {}
	ConstVecIterator(VectorType *MyVec, NodePtr ptr) : MyBase(MyVec, ptr) {}

	reference operator*() const
	{
		return ptr;
	}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}
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
	using const_iterator = ConstVecIterator<MyBase>;

	friend iterator;
	friend const_iterator;
	
	template<class, int, class>
	friend class SmallVec;

private:
	Allocator alloc;

	AlignedStorage MyData[alignedSize];

	unsigned int useAligned		 : 1;
	unsigned int copyFromAligned : 1;

	size_type MySize = 0;
	size_type MyCapacity = alignedSize;

	NodePtr MyBegin = nullptr;
	NodePtr MyLast = nullptr;
	NodePtr MyEnd = nullptr;

public:

	// In the case aligned size is <= 1, don't use aligned memory
	SmallVec() : useAligned(alignedSize > 1), 
			     copyFromAligned(false) 
	{
		if (useAligned)
		{
			MyBegin = MyLast = reinterpret_cast<NodePtr>(std::addressof(MyData[0]));
		}
	}

	iterator begin() { return iterator{ this, MyBegin }; }
	iterator end() { return iterator{ this, MyLast }; }
	const_iterator begin() const { return const_iterator{ this, MyLast }; }
	const_iterator end() const { return const_iterator{ this, MyLast }; }
	const_iterator cbegin() const { return const_iterator{ this, MyLast }; }
	const_iterator cend() const { return const_iterator{ this, MyLast }; }

	template<class T, int Sz, class Al>
	SmallVec& operator=(const SmallVec<T, Sz, Al>& other)
	{
		// This is here so we can convert a vector containing ints 
		// to a vector of size_t's for example
		using OtherNodePtr = typename SmallVec<T, Sz, Al>::NodePtr;

		for (NodePtr n = MyBegin; n <= MyLast; ++n)
			AlTraits::destroy(alloc, n);

		if (other.size() >= this->capacity()) // TODO: Include copy instructions if their size doesn't exceed our aligned storage size
		{
			auto nodePair = allocate(other.capacity()); // Should we use size of capacity here?

			if (!useAligned)
				AlTraits::deallocate(alloc, MyBegin, MyCapacity);
			else
				useAligned = false;

			MyBegin = nodePair.first;
			MyEnd = nodePair.second;
			MyCapacity = other.capacity();
		}

		MySize = other.size();
		MyLast = MyBegin + static_cast<difference_type>(MySize - 1);

		NodePtr mb = MyBegin;
		for(OtherNodePtr p = other.MyBegin; p <= other.MyLast; ++p, ++mb)
			AlTraits::construct(alloc, mb, *p);

		return *this;
	}

	reference operator[](const int idx)
	{
		return *(MyBegin + static_cast<difference_type>(idx));
	}

	const_reference operator[](const int idx) const
	{
		return *(MyBegin + static_cast<difference_type>(idx));
	}

private:

	void copyTo(NodePtr first) // TODO: More of a reallocate function, split it up so we can use the copy mechanics elsewhere!
	{
		NodePtr oldFirst = MyBegin;
		NodePtr oldLast = MyLast;		

		for (oldFirst; oldFirst <= oldLast; ++oldFirst, ++first)
		{
			*first = std::move(*oldFirst);
			AlTraits::destroy(alloc, oldFirst);
		}

		if (copyFromAligned)
			copyFromAligned = false;
		else
			AlTraits::deallocate(alloc, MyBegin, MyCapacity);	
	}

	std::pair<NodePtr, NodePtr> allocate(const size_type sz)
	{
		NodePtr first = alloc.allocate(sz);
		return { first, first + static_cast<difference_type>(sz) };
	}
	
	void grow()
	{
		const size_type newCapacity = MyCapacity + (MyCapacity + 1) / 2; // TODO: Revisit Growth Rates

		auto[first, last] = allocate(newCapacity);

		if(MySize - 1)
			copyTo(first);

		// If we were using AlignedStorage previously (or not!) this is where
		// MyBegin and MyEnd get switched over out of the AlignedStorage array
		MyCapacity = newCapacity;
		MyBegin = first;
		MyLast = MyBegin + static_cast<difference_type>(MySize - 1);
		MyEnd = last;
	}

public:

	void reserve(size_type newCap)
	{
		if (newCap <= MyCapacity)
			return;

		auto[first, last] = allocate(newCap);

		if (useAligned && newCap > alignedSize)
		{
			copyFromAligned = true;
			useAligned = false;
		}

		copyTo(first);
	}

	template<class... Args>
	reference emplace_back(Args&& ...args)
	{
		++MySize;

		if (MySize >= MyCapacity - 1)
		{
			if (useAligned)
			{
				useAligned = false;
				copyFromAligned = true;
			}

			grow();
		}

		// If this is the first insert
		// we don't actually want to increment last
		else
			if (MySize == 1)
				;
			else
				++MyLast;

		if (useAligned)
			new(MyLast) Type(std::forward<Args>(args)...);
		
		else	
			AlTraits::construct(alloc, MyLast, std::forward<Args>(args)...);

		return back();
	}

	void push_back(const Type& t)
	{
		emplace_back(t);
	}

	void push_back(Type&& t)
	{
		emplace_back(std::move(t));
	}

	void pop_back()
	{	// Don't call with an empty vector!
		AlTraits::destroy(alloc, MyLast); // TODO: Make sure this is okay when toDelete is in aligned storage!
		
		--MySize;
		--MyLast; 
	}

	bool empty() const noexcept
	{
		return static_cast<bool>(MySize);
	}

	size_type size() const noexcept
	{
		return MySize;
	}

	size_type max_size() const noexcept
	{
		return (std::min(static_cast<size_type>((numeric_limits<difference_type>::max)), AlTraits::max_size(alloc)));
	}

	size_type capacity() const noexcept
	{
		return MyCapacity;
	}

	reference back()
	{
		return *MyLast;
	}

	const_reference back() const
	{
		return *MyLast;
	}

	pointer data() noexcept
	{
		return MyBegin;
	}

	const_pointer data() const noexcept
	{
		return MyBegin;
	}

	void clear() noexcept // TODO:
	{
		NodePtr first = MyBegin;
		NodePtr last = MyBegin + MySize;

		for (first; first != last; ++first)
			AlTraits::destroy(alloc, first);

		MySize = 0;
	}
};