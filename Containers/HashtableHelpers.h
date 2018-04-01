#pragma once
#include "helpers.h"

namespace detail
{
	constexpr char empty = 0;
	constexpr char filled = 1;
	constexpr char deleted = 2;
}


// Wrapper for types needed by Hashtable
template<class Traits, template<class> class NodeT>
struct HashTypes
{
	using Alloc = typename Traits::allocator_type;
	using AllocTraits = std::allocator_traits<Alloc>;

	using hasher = typename Traits::hasher;
	using key_type = typename Traits::key_type;
	using get_key = typename Traits::get_key;

	using difference_type = typename AllocTraits::difference_type;
	using value_type = typename AllocTraits::value_type;
	using pointer = typename AllocTraits::pointer;
	using const_pointer = typename AllocTraits::const_pointer;
	using reference = value_type & ;
	using const_reference = const value_type&;

	using node_type = typename Traits::node_type;
	using size_type = size_t;

	using Node = NodeT<node_type>;
	using NodePtr = Node * ;

	using NodeAl = RebindAllocator<Alloc, Node>;
	using NodeAlTraits = std::allocator_traits<NodeAl>;

	struct get_hash
	{
		size_type operator()(const key_type& v) const
		{
			return hash(v);
		}

		hasher hash;
	};
};

template<class Table>
class HashIterator
{
public:

	using NodePtr = typename Table::NodePtr;
	using pointer = typename Table::pointer;
	using reference = typename Table::reference;

	HashIterator() = default;
	HashIterator(NodePtr ptr, const Table *table)
		: ptr(ptr), table(table)
	{
	}

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
		++ptr;
		for (ptr; ptr < table->end().ptr; ++ptr)
		{
			if (ptr->state & detail::filled)
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

	bool operator==(const HashIterator& other) const
	{
		return this->ptr == other.ptr;
	}

	bool operator!=(const HashIterator& other) const
	{
		return !(this->ptr == other.ptr);
	}

protected:
	friend Table;

	NodePtr ptr;
	const Table *table;
};

template<class Table>
class ConstHashIterator : public HashIterator<Table>
{
public:
	using MyBase = HashIterator<Table>;
	using NodePtr = typename Table::NodePtr;
	using pointer = typename Table::const_pointer;
	using reference = typename Table::const_reference;
	friend MyBase;

	ConstHashIterator() : MyBase() {};
	ConstHashIterator(const MyBase& it) // Regular to const iterator
		: MyBase(it)
	{
	}
	ConstHashIterator(NodePtr p, const Table *t)
		: MyBase(p, t)
	{
	}

	reference operator*() const
	{
		return this->ptr->data;
	}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}
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
		if (++ptr >= table->MyEnd) // Is this safe?
			ptr = table->MyBegin;

		return *this;
	}

	Table *table;
	NodePtr ptr;
};