#pragma once
#include "HashtableHelpers.h"


template<class NodeType>
struct RobinhoodNode
{
	int dist;

	size_t hash; // Should maybe use size_type template param?

	NodeType data;
};

template<class Traits, bool Multi>
class RobinhoodHash
{
	using MyBase = RobinhoodHash<Traits, Multi>;
	using BaseTypes = HashTypes<Traits, RobinhoodNode>;

	using Alloc		   = typename BaseTypes::Alloc;
	using AllocTraits  = typename BaseTypes::AllocTraits;
	using NodeAl	   = typename BaseTypes::NodeAl;
	using NodeAlTraits = typename BaseTypes::NodeAlTraits;

	using key_type  = typename BaseTypes::key_type;
	using get_hash  = typename BaseTypes::get_hash;
	using get_key	= typename BaseTypes::get_key;
	using Node		= typename BaseTypes::Node;
	using NodePtr   = typename BaseTypes::NodePtr;
	using node_type = typename BaseTypes::node_type;
	using size_type = typename BaseTypes::size_type;

	using key_equal  = typename Traits::key_equal;
	using node_equal = typename Traits::node_equal;

	using difference_type = typename BaseTypes::difference_type;
	using value_type	  = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer   = typename BaseTypes::const_pointer;
	using reference		  = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using wrapIterator = WrappingIterator<MyBase>;
	using iterator = HashIterator<MyBase>;

	friend wrapIterator;

	using PairIb = std::pair<iterator, bool>;

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

	bool isFilled(NodePtr p) const
	{
		return p->hash;
	}

	template<class... Val>
	void constructNode(NodePtr p, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);
	}

	void deallocate(NodePtr start, NodePtr end, const size_type size)
	{
		if (!start)
			return;

		for (NodePtr it = start; it != end; ++it)
			NodeAlTraits::destroy(nodeAl, it);

		NodeAlTraits::deallocate(nodeAl, start, size);
	}

	void reallocate(NodePtr& first, NodePtr& last)
	{
		std::swap(MyBegin, first);
		std::swap(MyEnd, last);

		for (NodePtr it = MyBegin; it != MyEnd; ++it) // TODO: A likely area for optimizations would a memset be able to set all these states to zero?
		{
			NodeAlTraits::construct(nodeAl, std::addressof(it->hash), 0);
		}

		for (NodePtr it = first; it != last; ++it)
		{
			if (isFilled(it))
			{
				//	PairIb place = emplaceWithHash(key, hash, MyBegin);
				constructNode(place.first.ptr, std::move(it->data));
			}
		}
	}

	void increaseCapacity() // TODO: Use a power of 2 bitmask throughout 
	{
		const size_type oldSize = MyCapacity;
		const size_type newSize = MyCapacity ? MyCapacity * 2 : 16;
		Node* b = nodeAl.allocate(newSize);
		Node* e = b + static_cast<difference_type>(newSize);

		MyCapacity = newSize; // Must be set first otherwise items are placed into incorrect buckets in new array

		reallocate(b, e);

		deallocate(b, e, oldSize); // TODO: Non-bulk deallocation?
	}
	
};