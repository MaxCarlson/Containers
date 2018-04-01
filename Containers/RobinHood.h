#pragma once
#include "HashtableHelpers.h"


template<class NodeType>
struct RobinhoodNode
{
	int dist;

	//size_t hash; // TODO: Make hash optional to store?

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
	friend iterator;

	using PairIb = std::pair<iterator, bool>;

	NodeAl nodeAl;

	NodePtr MyBegin;
	NodePtr MyEnd;

	size_type MySize = 0;
	size_type MyCapacity = 0;

	static constexpr float defaultLoadFactor = 0.65f;
	float maxLoadFactor = defaultLoadFactor;

	get_hash getHash;
	get_key  getKey;

	key_equal  keyEqual;
	node_equal nodeEqual;

	enum { EMPTY = -1 };

	bool isFilled(int dist) const noexcept// TODO: Pass ptr here for better readability? Any speed loss?
	{
		return dist > -1;
	}

	template<class... Val>
	void constructNode(NodePtr p, const int dist, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);
		NodeAlTraits::construct(nodeAl, std::addressof(p->dist), dist);
		++MySize;
	}

	void reallocate(NodePtr first, NodePtr last, const size_type oldSize)
	{
		std::swap(MyBegin, first);
		std::swap(MyEnd, last);

		for (NodePtr it = MyBegin; it != MyEnd; ++it)
		{	// TODO: 30% of time is spent here when just running insertions. Optimize?
			NodeAlTraits::construct(nodeAl, std::addressof(it->dist), EMPTY);
		}

		if (first)
		{
			for (NodePtr it = first; it != last; ++it)
			{
				if (isFilled(it->dist))
				{
					std::pair<key_type, size_type> kh = getKeyAndHash(it->data);

					emplaceWithHash<false>(kh.first, kh.second, std::move(it->data)); // TODO: Create sepperate non duplicate checking function to insert
				}

				NodeAlTraits::destroy(nodeAl, it); // TODO: This needs to be called for every element right? Or only elements that have been constructed?
			}

			NodeAlTraits::deallocate(nodeAl, first, oldSize);
		}
	}

	void increaseCapacity() 
	{
		const size_type oldSize = MyCapacity;
		const size_type newSize = MyCapacity ? MyCapacity * 2 : 16;
		Node* b = nodeAl.allocate(newSize);
		Node* e = navigate(newSize, b);

		MyCapacity = newSize; // Must be set first otherwise items are placed into incorrect buckets in new array

		reallocate(b, e, oldSize);
	}

	size_type getBucket(const size_type hash) const noexcept
	{
		return hash & (capacity() - 1);
	}

	NodePtr navigate(const size_type idx, const NodePtr first) const 
	{
		return first + static_cast<difference_type>(idx);
	}

	template<bool checkDupli, class... Args>
	PairIb emplaceWithHash(const key_type& k, const size_type hash, Args&&... args)
	{
		const size_type initial = getBucket(hash);

		int dist = 0;
		bool inserted = false;
		wrapIterator w(navigate(initial, MyBegin), this);

		for (w; dist <= w.ptr->dist; ++w)
		{
			if (checkDupli && keyEqual(getKey(w.ptr->data), k)) // TODO: Look into multi and make sure working correctly!
			{
				inserted = false; 		
				return PairIb { iterator{w.ptr, this}, false };
			}
			++dist;
		}

		return insertAtNode(dist, w, std::forward<Args>(args)...);
	}

	template<class... Args>
	PairIb insertAtNode(int dist, wrapIterator w, Args&&... args)
	{
		NodePtr pos = w.ptr;
		bool inserted = false;
		node_type nt = { std::forward<Args>(args)... };

		for (w;; ++w, ++dist)
		{
			if (!isFilled(w.ptr->dist))
			{
				constructNode(w.ptr, dist, std::move(nt));
				
				return PairIb { iterator{pos, this}, inserted }; // TODO: Iterator return correctness is broken after refactor. FIX!
			}
			else if (w.ptr->dist < dist)
			{
				if (!inserted)
				{   // Mark location of our original element emplacement 
					pos = w.ptr;
					inserted = true; 
				}

				if constexpr(!std::is_same_v<key_type, value_type>)
				{	// Get rid of const-ness of the key for just a moment so we can do a move-swap here
					std::swap(const_cast<key_type&>(nt.first), const_cast<key_type&>(w.ptr->data.first));
					std::swap(nt.second, w.ptr->data.second);
				}
				else
					std::swap(nt, w.ptr->data);

				std::swap(dist, w.ptr->dist);
			}
		}
		return PairIb { iterator { pos, this }, false };
	}

	template<class... Args>
	std::pair<key_type, size_type> getKeyAndHash(Args&& ...args)
	{
		const key_type k = getKey({ std::forward<Args>(args)... });
		return std::make_pair(k, getHash(k));
	}

	template<class... Args>
	PairIb tryEmplace(Args&&... args)
	{
		if (size() + 1 > maxLoadFactor * capacity())
			increaseCapacity();
		
		std::pair<key_type, size_type> kh = getKeyAndHash(std::forward<Args>(args)...);

		return emplaceWithHash<!Multi>(kh.first, kh.second, std::forward<Args>(args)...);
	}

public:

	template<class... Args>
	PairIb emplace(Args&&... args)
	{
		PairIb ib = tryEmplace(std::forward<Args>(args)...);

		return ib;
	}

	size_type size() const noexcept
	{
		return MySize;
	}

	size_type capacity() const noexcept
	{
		return MyCapacity;
	}
};