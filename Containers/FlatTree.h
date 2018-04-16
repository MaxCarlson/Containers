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

	using value_type = typename Traits::value_type;
	using key_type   = typename Traits::key_type;

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

private:
	NodeAl nodeAl;

	Storage MyData;

	size_type upperBound(const key_type &k) const // TODO: Benchmark against std::upperbound()!
	{
		size_type size = MyData.size();
		size_type low = 0;

		while (size > 0) 
		{
			size_type half = size / 2;
			size_type ohalf = size - half;
			size_type probe = low + half;
			size_type olow = low + ohalf;
			size = half;

			low = key_compare()(get_key()(MyData[probe]), k) ? olow : low;
		}

		return low;
	}

public:

	const NodeType & operator[](const key_type& k) 
	{
		
	}

	void reserve(size_type newCap)
	{
		MyData.reserve(newCap);
	}

	template<class... Args>
	void emplace(Args&& ...args)
	{
		// Temporary construction of a possible rvalue to lvalue
		// While we find it a place to sit
		Node n = Node{ std::forward<Args>(args)... };

		size_type idx = upperBound(get_key()(n));

		if (get_key()(MyData[idx]) == get_key()(n) && idx < MyData.size()) // TODO: Add template parameter to allow Multiples
			return;

		MyData.emplace(idx, std::move(n)); // TODO : Fix issues with MySize
	}

	const_iterator find(const key_type &k) const
	{
		const size_type idx = upperBound(k);

		if (get_key()(MyData[idx]) == k)
			return const_iterator{ &MyData, &MyData[idx] };

		return MyData.cend();
	}
};
