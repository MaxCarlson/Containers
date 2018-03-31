#pragma once
#include "HashtableHelpers.h"


template<class NodeType>
struct RobinhoodNode
{
	int dist;

	size_t hash; 

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

	bool isFilled(NodePtr p) const
	{
		return p->hash;
	}

	template<class... Val>
	void constructNode(NodePtr p, const size_type hash, const int dist, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);
		NodeAlTraits::construct(nodeAl, std::addressof(p->dist), dist);
		NodeAlTraits::construct(nodeAl, std::addressof(p->hash), hash);

		++MySize;
	}

	void reallocate(NodePtr first, NodePtr last, const size_type oldSize)
	{
		std::swap(MyBegin, first);
		std::swap(MyEnd, last);

		for (NodePtr it = MyBegin; it != MyEnd; ++it) 
		{
			NodeAlTraits::construct(nodeAl, &it->hash, 0);
		//	NodeAlTraits::construct(nodeAl, std::addressof(it->dist), 0); // TODO: This probably isn't completely necassary?>
		}

		if (first)
		{
			for (NodePtr it = first; it != last; ++it)
			{
				if (it->hash) //isFilled(it)
				{
					std::pair<const key_type, size_type> kh = getHashAndKey(it->data);

					emplaceWithHash(kh.first, kh.second, std::move(it->data));
				}

				NodeAlTraits::destroy(nodeAl, it); // TODO: This needs to be called for every element right? Or no?
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

	size_type getBucket(size_type hash) const noexcept
	{
		return hash & (capacity() - 1);
	}

	NodePtr navigate(const size_type idx, const NodePtr first) const 
	{
		return first + static_cast<difference_type>(idx);
	}
	/*
	template<class... Args>
	PairIb emplaceWithHash(const key_type& k, const size_type hash, Args&&... args)
	{
		const size_type initial = getBucket(hash);

		int dist = 0;
		bool inserted = false;
		NodePtr pos = navigate(initial, MyBegin);

		if (!pos->hash) // !isFilled(pos)
		{
			inserted = true;
			constructNode(pos, hash, dist, std::forward<Args>(args)...);
		}
		else
		{
			wrapIterator w(pos, this);
			++w;
			++dist;

			Node nt;
			nt.data = { std::forward<Args>(args)... };
			nt.hash = std::move(hash);

			for (w; w.ptr != pos; ++w, ++dist) 
			{
				if (!w.ptr->hash) // !isFilled(w.ptr)
				{
					constructNode(w.ptr, nt.hash, dist, std::move(nt.data));
					break;
				}
				else if (!Multi && keyEqual(k, getKey(w.ptr->data))) // TODO: Add template param to function that allows for overwriting old identical elements
					break; // No emplacement of identical elements. 
				
				else if (w.ptr->dist < dist)
				{
					if (!inserted)
					{   // Mark location of our original element emplacement 
						pos = w.ptr;  
						inserted = true;
					}

					std::swap(nt.data, w.ptr->data);
					std::swap(nt.hash, w.ptr->hash);
					std::swap(dist,    w.ptr->dist);
				}		
			}
		}

		return PairIb { iterator{pos, this}, inserted };
	}
	*/
	template<class... Args>
	PairIb emplaceWithHash(const key_type& k, const size_type hash, Args&&... args)
	{
		const size_type initial = getBucket(hash);

		int dist = 0;
		bool inserted = false;
		NodePtr pos = navigate(initial, MyBegin);

		Node nt;
		nt.data = { std::forward<Args>(args)... };
		nt.hash = std::move(hash);

		for (wrapIterator w(pos, this); ; ++w, ++dist)
		{
			if (!w.ptr->hash) // !isFilled(w.ptr)
			{
				constructNode(w.ptr, nt.hash, dist, std::move(nt.data));
				break;
			}
			else if (!Multi && keyEqual(k, getKey(w.ptr->data))) // TODO: Add template param to function that allows for overwriting old identical elements
				break; // No emplacement of identical elements. 

			else if (w.ptr->dist < dist)
			{
				if (!inserted)
				{   // Mark location of our original element emplacement 
					pos = w.ptr;
					inserted = true;
				}

				std::swap(nt.data, w.ptr->data);
				std::swap(nt.hash, w.ptr->hash);
				std::swap(dist, w.ptr->dist);
			}
		}

		return PairIb { iterator { pos, this }, inserted };
	}

	template<class... Args>
	std::pair<const key_type, size_type> getHashAndKey(Args&& ...args)
	{
		const key_type& k = getKey({ std::forward<Args>(args)... });
		return std::make_pair(k, getHash(k));
	}

	template<class... Args>
	PairIb tryEmplace(Args&&... args)
	{
		if (size() + 1 > maxLoadFactor * capacity())
			increaseCapacity();
		
		std::pair<const key_type, size_type> kh = getHashAndKey(std::forward<Args>(args)...);

		PairIb ib = emplaceWithHash(kh.first, kh.second, std::forward<Args>(args)...);

		return ib;
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