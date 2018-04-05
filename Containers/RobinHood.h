#pragma once
#include "HashtableHelpers.h"

template<class NodeType>
struct RobinhoodNode
{
	signed short dist : 16;

	//size_t hash; // TODO: Make hash optional to store?

	NodeType data;
};

template<class Table>
class RobinIterator : public HashIterator<Table>
{
public:
	using NodePtr   = typename Table::NodePtr;
	using pointer   = typename Table::pointer;
	using reference = typename Table::reference;
	using MyBase    = HashIterator<Table>;
	friend MyBase;

	RobinIterator() : MyBase() {}
	RobinIterator(NodePtr ptr, Table* table) 
		: MyBase(ptr, table) {}

	RobinIterator& operator++()
	{
		++this->ptr;
		for (this->ptr; this->ptr < this->table->MyEnd; ++this->ptr) // Broken if this->ptr == MyEnd
			if (this->ptr->dist != -1)
				break;
		
		return *this; 
	}

	RobinIterator operator++(int)
	{
		RobinIterator tmp = *this;
		++*this;
		return tmp;
	}

	RobinIterator& operator--() // Broken if this->ptr == MyBegin
	{
		--this->ptr;
		for (this->ptr; this->ptr > this->table->MyBegin; --this->ptr)
			if (this->ptr->dist != -1)
				break;

		return *this;
	}

	RobinIterator operator--(int)
	{ 
		RobinIterator tmp = *this;
		--*this;
		return tmp;
	}
};

template<class Table>
class ConstRobinIterator : public RobinIterator<Table>
{
public:
	using NodePtr   = typename Table::NodePtr;
	using pointer   = typename Table::const_pointer;
	using reference = typename Table::const_reference;
	using MyBase    = RobinIterator<Table>;
	friend MyBase;

	ConstRobinIterator(): MyBase() {}
	ConstRobinIterator(const MyBase& b) : MyBase(b) {}
	ConstRobinIterator(NodePtr p, Table* t) : MyBase(p, t) {}

	reference operator*() const
	{
		return this->ptr->data;
	}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}
};

template<class Traits, bool Multi>
class RobinhoodHash
{
public:
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
	using iterator = std::conditional_t<std::is_same<key_type, value_type>::value,
		ConstRobinIterator<MyBase>,
		RobinIterator<MyBase>>;

	using const_iterator = ConstRobinIterator<MyBase>;

	friend wrapIterator;
	friend iterator;
	friend const_iterator;
	friend RobinIterator<MyBase>;

	using PairIb = std::pair<iterator, bool>;
	using PairIs = std::pair<iterator, size_type>;

private:
	NodeAl nodeAl;

	NodePtr MyBegin = nullptr;
	NodePtr MyEnd = nullptr;

	size_type MySize = 0;
	size_type MyCapacity = 0;

	static constexpr float defaultLoadFactor = 0.65f;
	float maxLoadFactor = defaultLoadFactor;

	get_hash getHash;
	get_key  getKey;

	key_equal  keyEqual;
	node_equal nodeEqual;

	static constexpr short EMPTY = -1;

public:
	RobinhoodHash() { MySize = MyCapacity = 0; MyBegin = MyEnd = nullptr; }

private:

	bool isEmpty(const int dist) const noexcept // TODO: Pass ptr here for better readability? Any speed loss?
	{
		return dist == EMPTY;//!(dist & EMPTY);
	}

	template<class... Val>
	void constructNode(NodePtr p, const int dist, Val&& ...val) // TODO: Exception handling
	{
		NodeAlTraits::construct(nodeAl, std::addressof(p->data), std::forward<Val>(val)...);
		p->dist = dist;
		++MySize; 
	}

	void reallocate(NodePtr first, NodePtr last, const size_type oldSize)
	{
		std::swap(MyBegin, first);
		std::swap(MyEnd, last);

		for (NodePtr it = MyBegin; it <= MyEnd; ++it)	
			it->dist |= EMPTY;

		for (NodePtr it = first; it != last; ++it)
		{
			if (!isEmpty(it->dist))
			{
				const std::pair<key_type, size_type> kh = getKeyAndHash(it->data); // TODO: Repeditive, dual construction of obj; here and in insert

				emplaceWithHash<false>(kh.first, kh.second, std::move(it->data)); 
				NodeAlTraits::destroy(nodeAl, it); // TODO: This needs to be called only for constructed elements right?
			}
		}
		if (first)
		{
			std::cout << "ENewBegin " << first << " New End " << last << std::endl;
			NodeAlTraits::deallocate(nodeAl, first, oldSize);
		}
	}

	void increaseCapacity() 
	{
		const size_type oldSize = MyCapacity;
		const size_type newSize = MyCapacity ? MyCapacity * 2 : 16;

		std::cout << "\n";
		std::cout << "oldSize " << oldSize << " newSize " << newSize << std::endl;
		std::cout << "Begin     " << MyBegin << " End     " << MyEnd << std::endl;

		NodePtr b = nodeAl.allocate(newSize);
		NodePtr e = navigate(newSize, b);

		std::cout << "New Begin " << b << " New End " << e << std::endl;

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
		int dist = 0;
		wrapIterator w(navigate(getBucket(hash), MyBegin), this);

		// Find the first location this new node can be inserted
		for (w; dist <= w.ptr->dist; ++w, ++dist)
		{
			if (checkDupli && keyEqual(getKey(w.ptr->data), k)) // TODO: Look into multi and make sure working correctly!
			{
				return PairIb { iterator{w.ptr, this}, false };
			}
		}

		return insertAtNode(dist, w, std::forward<Args>(args)...);
	}

	void swapNodes(node_type& n, node_type& n1)
	{
		if constexpr(!std::is_same<key_type, value_type>::value)
		{	// Get rid of const-ness of the key for just a moment so we can do a move-swap here
			std::swap(const_cast<key_type&>(n.first), const_cast<key_type&>(n1.first));
			std::swap(n.second, n1.second);
		}
		else
			std::swap(n, n1);
	}

	template<class... Args>
	PairIb insertAtNode(int dist, wrapIterator w, Args&&... args)
	{
		NodePtr pos = w.ptr;
		node_type nt = { std::forward<Args>(args)... };

		// Insert new node and shift all other nodes 
		// (that we need to) over right with robinhood hashing
		for (w;; ++w, ++dist)
		{
			if (isEmpty(w.ptr->dist))
			{
				constructNode(w.ptr, dist, std::move(nt));		
				return PairIb { iterator{ pos, this }, true }; // TODO: Iterator return correctness is broken after refactor. FIX!
			}
			else if (w.ptr->dist < dist)
			{
				swapNodes(nt, w.ptr->data);

				const int tmp = w.ptr->dist;
				w.ptr->dist = std::move(dist);
				dist	    = std::move(tmp);
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
		
		std::pair<key_type, size_type> kh = getKeyAndHash(std::forward<Args>(args)...);   // TODO: We're doing 2 total obj constructions, one in getKeyAndHash 
																						  // and one in insert at node, Pass the object by & and move once created?
		return emplaceWithHash<!Multi>(kh.first, kh.second, std::forward<Args>(args)...); 
	}

	const_iterator locateElement(const key_type& k) 
	{
		NodePtr pos = navigate(getBucket(getHash(k)), MyBegin);

		for (wrapIterator w(pos, this);; ++w)
		{
			if (isEmpty(w.ptr->dist))
				return const_iterator { MyEnd, this };

			else if (keyEqual(getKey(w.ptr->data), k)) 
				return const_iterator { w.ptr, this };
		}
		return iterator { MyEnd, this };
	}

	iterator eraseElement(iterator it) // TODO: Exception Handling?
	{
		--MySize;
		NodeAlTraits::destroy(nodeAl, std::addressof(it.ptr->data));
		
		iterator ret = it;
		wrapIterator first(it.ptr, this);

		for (wrapIterator next(++it.ptr, this);; ++first, ++next)
		{
			if (next.ptr->dist > 0)
			{
				swapNodes(first.ptr->data, next.ptr->data);
				--first.ptr->dist;
			}
			else
			{	// Mark the last elements place (the old spot of the one just moved to the left) empty
				first.ptr->dist |= EMPTY;

				if (isEmpty(ret.ptr->dist))
					++ret;

				return ret;
			}
		}

		return ret; // Very unlikely/impossible?
	}

	PairIs eraseElements(const_iterator first, const_iterator end) // TODO: This can likely be done much faster if we mass delete
	{															   // and shift elements(move) over till they hit zero or we run out of space
		size_type num = 0;										   // instead of shifting and erasing one at a time
		for (iterator it = first; it != end; ++num)
			eraseElement(it);

		return PairIs { end, num };
	}


	iterator findFirst()
	{
		iterator i(MyBegin, this);

		if (isEmpty(i.ptr->dist))
			++i;

		return i;
	}

public:

	iterator begin() { return iterator { findFirst() }; }
	iterator end() { return iterator { MyEnd, this }; }
	const_iterator begin() const { return const_iterator { findFirst() }; }
	const_iterator end() const { return const_iterator { MyEnd, this }; }
	const_iterator cbegin() const { return const_iterator { findFirst() }; }
	const_iterator cend() const { return const_iterator { MyEnd, this }; }

	void max_load_factor(float f) noexcept
	{
		if (f < 0.98f && f > 0.0f)
			maxLoadFactor = f;
	}

	bool empty() const noexcept
	{
		return MySize;
	}

	size_type size() const noexcept
	{
		return MySize;
	}

	size_type capacity() const noexcept
	{
		return MyCapacity;
	}

	template<class... Args>
	PairIb emplace(Args&&... args)
	{
		PairIb ib = tryEmplace(std::forward<Args>(args)...);

		return ib;
	}

	iterator find(const key_type& k)
	{
		return locateElement(k);
	}

	const_iterator find(const key_type& k) const
	{
		return locateElement(k);
	}

	size_type erase(const key_type& k) // Need equal-range to use this with Multi
	{
		iterator it = find(k);
		size_type num = it.ptr != MyEnd;
		
		eraseElement(it);

		return num;
	}

	iterator erase(iterator it)
	{
		return eraseElement(it);
	}

	iterator erase(iterator first, iterator last) // Not Functional!
	{
		PairIs is = eraseElements(first, last);

		return is.first;
	}
};