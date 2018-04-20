#pragma once
#include "OrderedTypes.h"
#include "SmallVec.h"
#include <utility>

// Just a wrapper for compatibility with
// OrderedTypes wrapper
//
// use std::remove_const on NodeType if this wrapper causes perf penalty
template<class NodeType>
struct FlatTreeNode
{
	// Make the data we're holding non-const so we can swap/move
	using MyNodeType = typename std::remove_cv<NodeType>::type;
	MyNodeType data;

	FlatTreeNode() = default;
	FlatTreeNode(const NodeType& d) : data(d) {}
	FlatTreeNode(MyNodeType&& d) : data(std::move(d.data)) {}
	FlatTreeNode(FlatTreeNode&& t) : data(std::move(t.data)) {}

	template<class... Args>
	FlatTreeNode(Args&& ...args) : data(std::forward<Args>(args)...) {}

	operator NodeType&() noexcept { return data; } // TODO: This can be const, since NodeType will always be const ?
	operator NodeType() const noexcept { return data; }

	FlatTreeNode& operator=(const FlatTreeNode& f)
	{
		data = f.data;
		return *this;
	}

	FlatTreeNode& operator=(FlatTreeNode&& f) noexcept(std::is_nothrow_move_assignable<NodeType>::value) // TODO: Figure out how to use these noexcept conditions properly
	{
		data = std::move(f.data);
		return *this;
	}

};

template<class It>
struct FlatTreeIterator
{

};

// A sorted vector that uses 
// binary search when finding elements
// TODO: Add a Multi bool template param to allow for duplicates
template<class Traits>
class FlatTree
{
public:
	using MyBase = FlatTree<Traits>;
	
	template<class>
	friend class FlatTree;

	using value_type = typename Traits::value_type;
	using key_type   = typename Traits::key_type;

	// Are we a set or are we a map? Figure out what data type to use
	using NodeType = std::conditional_t<std::is_same_v<typename Traits::node_type, const key_type>, 
		typename Traits::key_type,
		std::pair<key_type, value_type>>;

	using BaseTypes = OrderedTypesNoNodeWrapper<Traits, NodeType>;

	using Node			  = typename BaseTypes::Node;
	using NodePtr		  = typename BaseTypes::NodePtr;
	using difference_type = typename BaseTypes::difference_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer	  = typename BaseTypes::const_pointer;
	using reference		  = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using key_compare	= typename Traits::key_compare;
	using value_compare = typename Traits::value_compare;
	using get_key		= typename Traits::get_key;
	using size_type		= size_t;

	using NodeAl	   = typename BaseTypes::NodeAl;
	using NodeAlTraits = typename BaseTypes::NodeAlTraits;

	using Storage = SmallVec<Node, 4, NodeAl>; // TODO: Revisit preallocated storage size

	using iterator = typename Storage::iterator;			 // TODO: Need to specialize non-const iterator for map types so value can be changed but not key!
	using const_iterator = typename Storage::const_iterator;

	iterator begin() noexcept { return MyData.begin(); }
	iterator end() noexcept { return MyData.end(); }
	const_iterator begin() const noexcept { return MyData.begin(); }
	const_iterator end() const noexcept { return MyData.end(); }
	const_iterator cbegin() const noexcept { return MyData.cbegin(); }
	const_iterator cend() const noexcept { return MyData.cend(); }

	using PairIb = std::pair<iterator, bool>;

private:

	Storage MyData;

	size_type binSearch(const key_type &k) const // TODO: Benchmark against std::upper_bound/others()!
	{
		size_type size = MyData.size();
		size_type low = 0;

		while (size > 0) 
		{
			size_type half  = size /  2;
			size_type ohalf = size -  half;
			size_type probe = low  +  half;
			size_type olow  = low  + ohalf;
			size = half;

			low = key_compare()(get_key()(MyData[probe]), k) ? olow : low;
		}
		return low;
	}

public:

	template<class T, typename = std::enable_if_t<std::is_convertible_v<Node, typename FlatTree<T>::Node>>>
	FlatTree& operator=(const FlatTree<T>& other)
	{
		MyData = other.MyData;
		return *this;
	}

	FlatTree& operator=(const FlatTree& other) 
	{
		MyData = other.MyData;
		return *this;
	}

	size_type size() const noexcept
	{
		return MyData.size();
	}

	bool empty() const noexcept
	{
		return MyData.empty();
	}

	size_type capacity() const noexcept
	{
		return MyData.capacity();
	}

	void clear() noexcept(std::is_nothrow_destructible_v<Node>)
	{
		MyData.clear();
	}

	void reserve(size_type newCap)
	{
		MyData.reserve(newCap);
	}

	iterator upper_bound(const key_type& k)
	{
		const size_type idx = binSearch(k);

		return iterator{ &MyData, &MyData[idx + 1] };
	}

	iterator lower_bound(const key_type& k)
	{
		const size_type idx = binSearch(k);

		return iterator{ &MyData, &MyData[idx] };
	}

	// O(Log N) find time 
	// O(N) insert time, N being the number of elements after inserted position.
	// O(1) insert time if element is the greatest of the predicate type 
	// (largest when using std::less for example)
	template<class... Args>
	PairIb emplace(Args&& ...args)
	{
		// Temporary construction of a node to possibly emplace
		// While we find it a place to sit
		Node n = Node{ std::forward<Args>(args)... };

		const size_type idx = binSearch(get_key()(n));

		if (get_key()(MyData[idx]) == get_key()(n) && idx < MyData.size()) // TODO: Add template parameter to allow Multiples
			return PairIb{ iterator{ &MyData, &MyData[idx] }, false };

		return PairIb{ MyData.emplace(idx, std::move(n)), true }; 
	}

	// O(Log N) find time unless correct position is it - 1. 
	// O(N) insert time, N being the number of elements after inserted position.
	// O(1) insert time if element is the greatest of the predicate type 
	// (largest when using std::less for example)
	template<class... Args>
	iterator emplace_hint(iterator it, Args&& ...args) // TODO: Use const_iterator here
	{
		Node n = Node{ std::forward<Args>(args)... };

		if (it == end() && !key_compare()(get_key()(n), get_key()(MyData.back())))
		{
			MyData.emplace_back(n);
			return iterator{ &MyData, &MyData.back() };
		}
		
		return emplace(std::move(n)).first;
	}

	// O(Log N) complexity
	iterator find(const key_type &k)
	{
		const size_type idx = binSearch(k);

		if (get_key()(MyData[idx]) == k)
			return iterator{ &MyData, &MyData[idx] };

		return MyData.cend();
	}
};
