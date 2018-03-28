#pragma once
#include <memory>
#include "helpers.h"

namespace detail
{
	constexpr char empty = 0;
	constexpr char filled = 1;
	constexpr char deleted = 2;
}

template<class node_type>
struct HashNode
{
	unsigned char state;

	node_type data;
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

	using node_type = typename Traits::node_type;
	using size_type = size_t;

	using Node = HashNode<node_type>;
	using NodePtr = Node*;

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

template<class Traits, bool Multi>
class OpenAddressLT
{
	using MyBase = OpenAddressLT<Traits, Multi>;
	using BaseTypes = HashTypes<Traits>;

	using Alloc		   = typename BaseTypes::Alloc;
	using AllocTraits  = typename BaseTypes::AllocTraits;
	using NodeAl	   = typename BaseTypes::NodeAl;
	using NodeAlTraits = typename BaseTypes::NodeAlTraits;

	using key_type  = typename BaseTypes::key_type;
	using get_hash  = typename BaseTypes::get_hash;
	using get_key   = typename BaseTypes::get_key;
	using Node	    = typename BaseTypes::Node;
	using NodePtr   = typename BaseTypes::NodePtr;
	using node_type = typename BaseTypes::node_type;
	using size_type = typename BaseTypes::size_type;

	using key_equal   = typename Traits::key_equal;
	using node_equal  = typename Traits::node_equal;

	using difference_type = typename BaseTypes::difference_type;
	using value_type      = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer   = typename BaseTypes::const_pointer;
	using reference       = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using wrapIterator = WrappingIterator<MyBase>;
	friend wrapIterator;

public:	

	OpenAddressLT()
	{
		MyBegin = MyEnd = nullptr;
	}

	using iterator = HashIterator<MyBase>;
	using PairIb = std::pair<iterator, bool>;
	using PairIt = std::pair<iterator, iterator>;
	using PairIs = std::pair<iterator, size_type>;

	friend iterator;

	iterator begin() { return iterator { MyBegin, this }; }
	iterator end() { return iterator { MyEnd, this }; };

private:
	NodeAl nodeAl;

	NodePtr MyBegin;
	NodePtr MyEnd;

	size_type MySize = 0;
	size_type MyCapacity = 0;

	static constexpr float defaultLoadFactor = 0.85f;
	float maxLoadFactor = defaultLoadFactor;

	get_hash getHash;
	get_key  getKey;

	key_equal  keyEqual;
	node_equal nodeEqual;


	// Delete these when done testing
	// 
	long long totalEmplace = 0;
	long long totalCollisions = 0;
	long long totalDistOnCol = 0;

	void deallocate(NodePtr start, NodePtr end, const size_type size)
	{
		if (!start)
			return;

		for (NodePtr it = start; it != end; ++it)
			NodeAlTraits::destroy(nodeAl, it);

		NodeAlTraits::deallocate(nodeAl, start, size);
	}

	// Copy current contents into new array,
	// pointed at by first, last and swap their values
	void reallocate(NodePtr& first, NodePtr& last) 
	{
		std::swap(MyBegin, first);
		std::swap(MyEnd, last);

		for (NodePtr it = MyBegin; it != MyEnd; ++it) // TODO: A likely area for optimizations
		{
			NodeAlTraits::construct(nodeAl, std::addressof(it->state), detail::empty);
		}

		for (NodePtr it = first; it != last; ++it)
		{
			if (isFilled(it)) // TODO: Cache Hashes? Would definitely provide speedup for hard-to-hash elements (not all that much space?)
			{
				const auto[key, hash] = getHashAndKey(it->data);

				emplaceWithHash(key, hash, MyBegin);
			}
		}
	}

	void printCollisionInfo(size_type oldSize) // Just for testing collisions et al
	{
		if (!totalEmplace)
			return;

		double percCol = (100.0 / double(totalEmplace)) * double(totalCollisions);
		double avgDistCol = double(totalDistOnCol) / double(totalCollisions);
		double avgDist = double(totalDistOnCol) / double(totalEmplace);

		std::cout << "\n" << "For Size: " << oldSize;
		std::cout << "\n" << "Total Inserts: " << totalEmplace;
		std::cout << "\n" << "Total Collissions: " << totalCollisions;
		std::cout << "\n" << "% Collisions: " << percCol;
		std::cout << "\n" << "Average Dist per Collison: " << avgDistCol;
		std::cout << "\n" << "Average Dist per Insert: " << avgDist;
		std::cout << "\n";

		totalCollisions = 0;
		totalDistOnCol = 0;
		totalEmplace = 0;
	}

	void increaseCapacity() // TODO: Use a power of 2 bitmask throughout 
	{
		const size_type oldSize = MyCapacity;
		const size_type newSize = MyCapacity ? MyCapacity * 2: 16;
		Node* b = nodeAl.allocate(newSize);
		Node* e = b + static_cast<difference_type>(newSize);

		MyCapacity = newSize; // Must be set first otherwise items are placed into incorrect buckets in new array

		reallocate(b, e);

		deallocate(b, e, oldSize); // TODO: Non-bulk deallocation?

		//printCollisionInfo(oldSize);
	}

	template<class... Val>
	void constructNode(NodePtr p, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);
	}

	bool isFilled(NodePtr p) const noexcept
	{
		return p->state & detail::filled; 
	}

	bool emptyOrDeleted(const int state) const noexcept
	{
		return (state ^ detail::filled)
			 | (state & detail::deleted);
	}

	void setStateFilled(NodePtr p)
	{
		p->state = detail::filled;
	}

	size_type getBucket(const size_type hash) const noexcept 
	{
		return hash & (capacity() - 1);
	}

	template<class... Args>
	std::pair<const key_type&, size_type> getHashAndKey(Args&& ...args)
	{
		const key_type& k = getKey({ std::forward<Args>(args)... });
		return std::make_pair(k, getHash(k));
	}

	NodePtr navigate(const size_type idx, const NodePtr first) const //noexcept?
	{
		return first + static_cast<difference_type>(idx);
	}

	PairIb emplaceWithHash(const key_type& key, const size_type hash, NodePtr first) 
	{
		const size_type bucket = getBucket(hash); // Excessive calls to key constructor here! Fix!

		NodePtr b = navigate(bucket, first); 

		++totalEmplace; //Testing param

		bool inserted = false;

		if (emptyOrDeleted(b->state))
		{
			setStateFilled(b);
			inserted = true;
		}
		else
		{
			wrapIterator w(b, this);
			++w;

			int i = 1; //Testing param
			for (w; w.ptr != b; ++w)
			{
				if (!Multi && keyEqual(getKey(w.ptr->data), key))
				{
					b = w.ptr;
					break;
				}
				else if (emptyOrDeleted(w.ptr->state))
				{
					b = w.ptr;
					setStateFilled(b);
					totalDistOnCol += i; //Testing param
					inserted = true;
					break;
				}
				++i; //Testing param
			}

			++totalCollisions; //Testing param
		}

		return PairIb { iterator{b, this}, inserted };
	}

	template<class... Val> 
	PairIb tryEmplace(Val&& ...val)
	{	
		if (size() + 1 > maxLoadFactor * capacity())
		{
			increaseCapacity();
		}

		const auto[key, hash] = getHashAndKey(val...);

		PairIb ib = emplaceWithHash(key, hash, MyBegin);

		if (ib.second) // If this is a true insertion and not a found element iterator
		{
			constructNode(ib.first.ptr, std::forward<Val>(val)...);
			++MySize;
		}

		return ib;
	}

	NodePtr getElementFromKey(const key_type &k) 
	{
		const size_type bucket = getBucket(getHash(k));

		NodePtr p = navigate(bucket, MyBegin);

		if (isFilled(p) && keyEqual(getKey(p->data), k))
			;
		else
		{
			wrapIterator w(p, this);
			++w; // Get an iterator to the next element(wrapping)

			bool found = false;
			for (w; w.ptr != p; ++w)
			{
				if (!isFilled(w.ptr))
					break;

				else if (keyEqual(getKey(w.ptr->data), k))
				{
					found = true;
					p = w.ptr;
					break;
				}
			}

			if (!found)
				p = MyEnd;
		}

		return p;
	}

	PairIt getEqualRange(const key_type& k)
	{
		NodePtr start = getElementFromKey(k);

		iterator it = iterator { start, this };
		PairIt its = { it, it };

		if (start != MyEnd)
		{
			wrapIterator w(start, this);
			++w;

			NodePtr end = start;
			for (w; w.ptr != start; end = w.ptr, ++w)
				if (!nodeEqual(w.ptr->data, start->data) || !isFilled(w.ptr))
					break;

			its.second = iterator{ end, this };
		}
		
		return its;
	}

	void deconstructNode(NodePtr p) // Deconstruct and mark area as deleted
	{
		NodeAlTraits::destroy(nodeAl, p);
		p->state = detail::deleted;
	}

	PairIs eraseElements(PairIt pit) // make const iterator?
	{
		PairIs is = { pit.second, 0 };

		for (iterator it = pit.first; it != pit.second; ++it, ++is.second) // make sure iterator points to next element!!!
			deconstructNode(it.ptr);

		MySize -= is.second;

		return is;
	}

public:

	size_type size() const noexcept
	{
		return MySize;
	}

	size_type capacity() const noexcept
	{
		return MyCapacity;
	}

	template<class... Val>
	PairIb emplace(Val&& ...val)
	{
		return tryEmplace(std::forward<Val>(val)...);
	}

	template<class K>
	iterator find(const K& k) 
	{
		NodePtr p = getElementFromKey(k);

		return iterator(p, this);
	}

	template<class K>
	PairIt equal_range(const K& k)
	{
		return getEqualRange(k);
	}

	size_type erase(const key_type& k)
	{
		PairIt its;

		if constexpr (Multi) // Test this to make sure it works!
			its = equal_range(k);

		else
			its.first = its.second = find(k);

		PairIs is = eraseElements(its);

		return is.second;
	}

};