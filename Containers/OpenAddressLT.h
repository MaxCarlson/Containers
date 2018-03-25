#pragma once
#include <memory>
#include "helpers.h"

namespace detail
{
	constexpr char empty = 0;
	constexpr char filled = 1;
	constexpr char deleted = 2;
}


template<class value_type>
struct HashNode
{
	char state;

	value_type data;
};

// Wrapper for types needed by Hashtable
template<class Traits>
struct HashTypes
{
	using Alloc = typename Traits::allocator_type;
	using AllocTraits = std::allocator_traits<Alloc>;

	using hasher    = typename Traits::hasher;
	using key_type  = typename Traits::key_type;
	using get_key   = typename Traits::get_key;

	using difference_type = typename AllocTraits::difference_type;
	using value_type	  = typename AllocTraits::value_type;
	using pointer		  = typename AllocTraits::pointer;
	using const_pointer   = typename AllocTraits::const_pointer;
	using reference		  = value_type & ;
	using const_reference = const value_type&;

	using Node = HashNode<value_type>;
	using NodePtr = Node*;

	using NodeAl = RebindAllocator<Alloc, Node>;
	using NodeAlTraits = std::allocator_traits<NodeAl>;

	struct get_hash
	{
		size_t operator()(const value_type& v) const
		{
			return hash(get(v));
		}

		hasher hash;
		get_key get;
	};
};

template<class Table>
struct HashIterator
{
	using NodePtr   = typename Table::NodePtr;
	using pointer   = typename Table::pointer;
	using reference = typename Table::reference;

	HashIterator() = default;
	HashIterator(NodePtr ptr, Table *table) 
		: ptr(ptr), table(table) {}

	reference operator*()
	{
		return ptr->data;
	}

	pointer operator->()
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	HashIterator& operator++()
	{
		for (NodePtr p = ptr; ptr < table->end->ptr; ++ptr)
		{
			if (ptr == table->end->ptr || ptr->state & filled)
				break;
		}

		return *this;
	}

	HashIterator operator++(int)
	{
		HashIterator tmp = *this;
		++*this;
		return tmp;
	}


protected:
	friend Table;

	NodePtr ptr;
	Table *table;
};

template<class Table>
struct WrappingIterator 
{
	using NodePtr = typename Table::NodePtr;
	using pointer = typename Table::pointer;
	using reference = typename Table::reference;

	WrappingIterator(NodePtr ptr, Table* table) : ptr(ptr), table(table) {}

	reference operator*()
	{
		return ptr->data;
	}

	pointer operator->()
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	WrappingIterator& operator++()
	{
		if (++ptr == table->MyEnd)
			ptr = table->MyBegin;

		return *this;
	}

	Table *table;
	NodePtr ptr;
};

// Open address linear probing table

template<class Traits>
class OpenAddressLT
{
	using MyBase = OpenAddressLT<Traits>;
	using BaseTypes = HashTypes<Traits>;

	using Alloc		   = typename BaseTypes::Alloc;
	using AllocTraits  = typename BaseTypes::AllocTraits;
	using NodeAl	   = typename BaseTypes::NodeAl;
	using NodeAlTraits = typename BaseTypes::NodeAlTraits;

	using key_type = typename BaseTypes::key_type;
	using get_hash = typename BaseTypes::get_hash;
	using get_key  = typename BaseTypes::get_key;
	using Node	   = typename BaseTypes::Node;
	using NodePtr  = typename BaseTypes::NodePtr;

	using key_equal   = typename Traits::key_equal;
	using value_equal = typename Traits::value_equal;

	using difference_type = typename BaseTypes::difference_type;
	using value_type      = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer   = typename BaseTypes::const_pointer;
	using reference       = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using wrapIterator = WrappingIterator<MyBase>;
	friend wrapIterator;

public:	
	using iterator = HashIterator<MyBase>;

	friend iterator;

	OpenAddressLT()
	{

	}

private:
	NodeAl nodeAl;

	NodePtr MyBegin;
	NodePtr MyEnd;

	size_t MySize = 0;
	size_t MyCapacity = 0;

	static constexpr float defaultLoadFactor = 0.85f;
	float maxLoadFactor = defaultLoadFactor;

	get_hash getHash;
	get_key  getKey;

	key_equal testEqual;

	void allocateStorage(size_t s)
	{
		if (s)
		{
			MyBegin = nodeAl.allocate(s);
			MyEnd = MyBegin + static_cast<difference_type>(s);

			for (NodePtr p = MyBegin; p != MyEnd; ++p)
			{
				NodeAlTraits::construct(nodeAl, std::addressof(p->state), detail::empty);
			}

			MyCapacity = s;
		}	
	}

	void deallocate(NodePtr p, NodePtr end)
	{
		for (NodePtr it = p; p != end; ++p)
			NodeAlTraits::destroy(nodeAl, it);

		NodeAlTraits::deallocate(nodeAl, p, end - p);
	}

	// Copy current contents into new array,
	// pointed at by first, last
	void copy(NodePtr first, NodePtr last)
	{
		for (NodePtr it = MyBegin; it != MyEnd; ++it)
		{
			if (isFilled(it)) // TODO: Cache Hashes?
			{
				const size_t hash = getHash(it->data);
				emplaceWithHash(hash, first, std::move(it->data));
			}
		//	else
		//		NodeAlTraits::construct(nodeAl, (first + static_cast<difference_type>(it - MyBegin)), detail::empty);
		}
	}

	void increaseCapacity() // TODO: Use a power of 2 bitmask throughout 
	{
		const size_t newSize = MyCapacity ? MyCapacity * 2 : 16;
		Node* b = nodeAl.allocate(newSize);
		Node* e = b + static_cast<difference_type>(newSize);

		copy(b, e);

		deallocate(MyBegin, MyEnd); // TODO: Non-bulk deallocation?

		MyBegin = b;
		MyEnd	= e;
		MyCapacity = newSize;
	}

	template<class... Val>
	void constructNode(NodePtr p, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);

		++MySize;
	}

	bool isFilled(NodePtr p) const noexcept
	{
		return p->state & detail::filled; 
	}

	bool emptyOrDeleted(NodePtr p) const noexcept
	{
		return ! (p->state & detail::filled) 
			   | (p->state & detail::deleted);
	}

	void setStateFilled(NodePtr p)
	{
		p->state = detail::filled;
	}

	size_t getBucket(const key_type& k) const // TODO: Use power 2 bitmask
	{
		return getHash(k) % MyCapacity;
	}

	NodePtr navigate(const size_t idx) const
	{
		return MyBegin + static_cast<difference_type>(idx);
	}

	NodePtr navigate(const size_t idx, const NodePtr first) const
	{
		return first + static_cast<difference_type>(idx);
	}

	template<class... Val>
	NodePtr emplaceWithHash(const size_t hash, NodePtr first, Val&& ...val)
	{
		const size_t bucket = getBucket(getKey(std::forward<Val>(val)...));

		NodePtr b = navigate(bucket, first); // bucket - 1?

		if (emptyOrDeleted(b))
			setStateFilled(b);
		else
		{
			wrapIterator w(b, this);
			++w;

			for (w; w.ptr != b; ++w) // TODO: Exception handling
				if (emptyOrDeleted(w.ptr))
				{
					b = w.ptr;
					setStateFilled(b);
					break;
				}
		}

		return b;
	}

	template<class... Val> 
	NodePtr tryEmplace(Val&& ...val)
	{	// Get a pointer to where we're going to put element

		if (size() + 1 > maxLoadFactor * capacity())
		{
			increaseCapacity();
		}

		const size_t thash = getHash(std::forward<Val>(val)...);

		NodePtr b = emplaceWithHash(thash, MyBegin, std::forward<Val>(val)...);

		return b;
	}

	NodePtr getElementFromKey(const key_type &k) 
	{
		const size_t bucket = getBucket(k);

		NodePtr p = navigate(bucket);

		if (testEqual(getKey(p->data), k))
			;
		else
		{
			wrapIterator w(p, this);
			++w; // Get an iterator to the next element(wrapping)

			bool found = false;
			for (w; w.ptr != p; ++w)
				if (testEqual(getKey(w.ptr->data), k))
				{
					found = true;
					p = w.ptr;
					break;
				}	

			if (!found)
				p = MyEnd;
		}

		return p;
	}

public:

	size_t size() const noexcept
	{
		return MySize;
	}

	size_t capacity() const noexcept
	{
		return MyCapacity;
	}

	template<class... Val>
	iterator emplace(Val&& ...val)
	{
		NodePtr p = tryEmplace(std::forward<Val>(val)...);

		if (p)
			constructNode(p, std::forward<Val>(val)...);

		return iterator(p, this);
	}

	iterator find(const key_type& k) 
	{
		NodePtr p = getElementFromKey(k);

		return iterator(p, this);
	}

};