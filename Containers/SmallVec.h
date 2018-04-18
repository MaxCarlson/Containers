#pragma once
#include "helpers.h"
#include <memory>

template<class Traits>
class FlatTree;

template<class VectorType> // TODO: Create common base so iterators/const_iterators can be added and subtracted with eachother
struct VecIterator 
{
public:
	using NodePtr		  = typename VectorType::NodePtr;
	using difference_type = typename VectorType::difference_type;
	using reference		  = typename VectorType::reference;
	using pointer		  = typename VectorType::pointer;

	VecIterator() = default;
	VecIterator(const VectorType *MyVec, NodePtr ptr) : MyVec(MyVec), ptr(ptr) {}

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

	/*
	difference_type operator+(const VecIterator& it) const
	{
		return this->ptr + *it;
	}

	difference_type operator-(const VecIterator& it) const
	{
		return this->ptr - *it;
	}
	*/

	bool operator==(const VecIterator& other) const
	{
		return this->ptr == other.ptr;
	}

	bool operator!=(const VecIterator& other) const
	{
		return !(this->ptr == other.ptr);
	}
//private:
	NodePtr ptr;
	const VectorType *MyVec;
};

template<class VectorType>
struct ConstVecIterator : public VecIterator<VectorType>
{
	using MyBase = VecIterator<VectorType>;
	using NodePtr = typename VectorType::NodePtr;
	using reference = typename VectorType::const_reference;
	using pointer = typename VectorType::const_pointer;

	ConstVecIterator() : MyBase() {};
	ConstVecIterator(const MyBase &b) : MyBase(b) {};
	ConstVecIterator(const VectorType *MyVec, NodePtr ptr) : MyBase(MyVec, ptr) {}

	reference operator*() const
	{
		return this->ptr;
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

	template<class>
	friend class FlatTree;

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
	SmallVec() : useAligned(alignedSize > 0), 
			     copyFromAligned(false) 
	{
		if (useAligned)
		{
			MyBegin = MyLast = reinterpret_cast<NodePtr>(std::addressof(MyData[0]));
			MyEnd = reinterpret_cast<NodePtr>(std::addressof(MyData[alignedSize]));
		}
	}

	iterator begin() noexcept { return iterator{ this, MyBegin }; }
	iterator end() noexcept { return iterator{ this, MyLast + 1 }; }
	const_iterator begin() const noexcept { return const_iterator{ this, MyBegin }; }
	const_iterator end() const noexcept { return const_iterator{ this, MyLast + 1 }; }
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }

	template<class T, int Sz, class Al>
	SmallVec& operator=(const SmallVec<T, Sz, Al>& other)
	{
		// This is here so we can copy vectors the require only one
		// type conversion from the compiler
		using OthersNodePtr = typename SmallVec<T, Sz, Al>::NodePtr;

		for (NodePtr n = MyBegin; n <= MyLast; ++n)
			AlTraits::destroy(alloc, n);

		if (other.size() >= this->capacity()) 
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
		for(OthersNodePtr p = other.MyBegin; p <= other.MyLast; ++p, ++mb)
			AlTraits::construct(alloc, mb, *p);

		return *this;
	}

	// TODO: 
	// template<class T, class Al>
	// SmallVec& operator=(const std::vector<T, Al> &other)

	// TODO:
	// template<class T, int Sz, class Al>
	// void swapAll(SmallVec<T, Sz, Al>& other)

	reference operator[](const int idx)
	{
		return *(MyBegin + static_cast<difference_type>(idx));
	}

	const_reference operator[](const int idx) const
	{
		return *(MyBegin + static_cast<difference_type>(idx));
	}

private:

	size_type calculateGrowth(size_type currentSize)
	{
		return currentSize + (currentSize + 2) / 2;
	}

	void copyTo(NodePtr first) noexcept(std::is_nothrow_destructible_v<Type> 
							        && (std::is_nothrow_copy_assignable_v<Type> 
									||  std::is_nothrow_move_assignable_v<Type>))
	{
		NodePtr oldFirst = MyBegin;
		NodePtr oldLast = MyLast;		

		///*
		for (oldFirst; oldFirst <= oldLast; ++oldFirst, ++first)
		{
			*first = std::move_if_noexcept(*oldFirst);
			AlTraits::destroy(alloc, oldFirst);
		}
		//*/
		//uncheckedMove(oldFirst, oldLast + 1, first);

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
		const size_type newCapacity = MyCapacity + (MyCapacity + 2) / 2; // TODO: Revisit Growth Rates

		auto[first, last] = allocate(newCapacity);

		if (useAligned)
		{	// Copy out of our aligned storage if we were using it
			// If this is the case we won't use it after this copy
			// Except if shrink_to_fit applies
			useAligned = false;
			copyFromAligned = true;
		}

		if(MySize)
			copyTo(first); 

		// If we were using AlignedStorage previously (or not!) this is where
		// MyBegin and MyEnd get switched over out of the AlignedStorage array
		MyCapacity = newCapacity;
		MyBegin = first;
		MyLast = MyBegin + static_cast<difference_type>(MySize);
		MyEnd = last;
	}

public:

	void reserve(size_type newCap)
	{
		if (newCap <= MyCapacity)
			return;

		auto[first, last] = allocate(newCap);

		if (useAligned) // && newCap > alignedSize -- Is implicit
		{
			copyFromAligned = true;
			useAligned = false;
		}

		copyTo(first);

		MyCapacity = newCap;
		MyBegin = first;
		MyLast = MyBegin + static_cast<difference_type>(MySize - 1);
		MyEnd = last;
	}

	template<class... Args>
	iterator emplace(size_type idx, Args&& ...args)
	{
		const_iterator it(this, MyBegin + static_cast<difference_type>(idx));
		return emplace(it, std::forward<Args>(args)...);
	}

	template<class... Args>
	iterator emplace(const_iterator it, Args&& ...args)
	{
		const size_type idx = it.ptr - MyBegin;

		if (MySize >= MyCapacity)
			grow();

		else if (MySize)
			++MyLast;

		++MySize;
		
		NodePtr place = MyBegin + static_cast<difference_type>(idx);
		if (place > MyLast || (MySize == 1 && place == MyBegin))
			 ;
		else
		{
			NodePtr newLast = MyLast;
			// TODO: Test std::move vs std::swap!
			for (NodePtr oldLast = MyLast - 1; oldLast >= place; --oldLast, --newLast)
				*newLast = std::move(*oldLast);
		}

		AlTraits::construct(alloc, place, std::forward<Args>(args)...);

		return iterator{ this, place };
	}

	template<class... Args>
	reference emplace_back(Args&& ...args)
	{
		if (MyLast >= MyEnd - 1)
			grow();
		
		// If this is the first insert or we're growing
		// we don't actually want to increment last
		else if(MySize > 0)
			++MyLast;
		
		++MySize;

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

	iterator erase(iterator pos) // TODO: use const_iterator
	{
		uncheckedMove(pos.ptr + 1, MyLast + 1, pos.ptr);

		return pos;
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

	void clear() 
	{
		NodePtr first = MyBegin;
		NodePtr last = MyBegin + MySize;

		for (first; first != last; ++first)
			AlTraits::destroy(alloc, first);

		MySize = 0;
	}
};