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

private:
	NodeAl nodeAl;

	NodePtr MyBegin;
	NodePtr MyEnd;

	size_t MySize = 0;
	size_t MyCapacity = 0;

	get_hash getHash;

public:
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

	template<class... Val>
	void constructNode(Val&& ...val)
	{

	}

	bool emptyOrDeleted(NodePtr p)
	{
		return !(p->state & detail::filled) 
			 | (p->state & detail::deleted);
	}

	void setStateFilled(NodePtr p)
	{
		p->state = detail::filled;
	}

	template<class Val>
	NodePtr tryEmplace(Val&& val)
	{	// Get a pointer to where we're going to put element
		const size_t thash = getHash(val);

		size_t bucket = thash % MyCapacity;

		NodePtr b = MyBegin + static_cast<difference_type>(bucket); // bucket - 1?

		if (emptyOrDeleted(b))
			setStateFilled(b);
		else
			for (wrapIterator w(b, this); w.ptr != b; ++w)
				if (emptyOrDeleted(w.ptr))
				{
					b = w.ptr;
					setStateFilled(b);
					break;
				}		

		return b;
	}

	template<class... Val>
	iterator emplace(Val&& ...val)
	{
		NodePtr p = tryEmplace(std::forward<Val>(val)...);


		return iterator(p, this);
	}

};