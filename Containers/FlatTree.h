#pragma once
#include "OrderedTypes.h"
#include <utility>


// Just a wrapper for compatibility with
// OrderedTypes wrapper
//
// use std::remove_const on NodeType if this wrapper causes perf penalty
template<class NodeType>
struct FlatTreeNode
{
	using MyNodeType = typename std::remove_cv<NodeType>::type;
	MyNodeType data;

	FlatTreeNode() = default;
	FlatTreeNode(const NodeType& d) : data(d) {}
	FlatTreeNode(MyNodeType&& d) : data(std::move(d.data)) {}
	FlatTreeNode(FlatTreeNode&& t) : data(std::move(t.data)) {}

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

// A sorted vector that uses 
// binary search when finding elements
// TODO: Add a Multi bool template param to allow for duplicates
template<class Traits>
class FlatTree
{
public:
	using MyBase = FlatTree<Traits>;
	using BaseTypes = OrderedTypes<Traits, FlatTreeNode>;

	using Node			  = typename BaseTypes::Node;
	using NodePtr		  = typename BaseTypes::NodePtr;
	//using NodeEqual		  = typename Traits::node_equal;
	using NodeType		  = typename BaseTypes::node_type;
	using difference_type = typename BaseTypes::difference_type;
	using value_type	  = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer	  = typename BaseTypes::const_pointer;
	using reference		  = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using key_type		= typename BaseTypes::key_type;
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

	NodeType & operator[](const key_type& k) 
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
		Node&& n(std::forward<Args>(args)...);

		size_type idx = upperBound(get_key()(n));

		if (MyData[idx] == n && idx < MyData.size()) // TODO: Add template parameter to allow Multiples
			return;

		MyData.emplace(idx, std::move(n)); // TODO : Fix issues with MySize
	}

	const_iterator find(const key_type &k) 
	{
		const size_type idx = upperBound(k);

		auto& kk = MyData[idx];

		if (get_key()(MyData[idx]) == k)
			return const_iterator{ &MyData, &MyData[idx] };

		return MyData.cend();
	}
};
