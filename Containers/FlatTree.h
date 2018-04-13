#pragma once
#include "OrderedTypes.h"
#include <utility>


// Just a wrapper for compatibility with
// OrderedTypes wrapper
template<class NodeType>
struct FlatTreeNode
{
	NodeType data;

	FlatTreeNode() = default;
	FlatTreeNode(const NodeType& d) : data(d) {}
	FlatTreeNode(NodeType&& d) : data(std::move(d)) {}
	FlatTreeNode(FlatTreeNode&& t) : data(std::move(t.data)) {}


	/*
	NodeType& operator*()
	{
		return data;
	}
	*/

	FlatTreeNode& operator=(const FlatTreeNode& f)
	{
		return (*this = f);
	}

	FlatTreeNode& operator=(FlatTreeNode&& f) noexcept(std::is_nothrow_move_assignable<NodeType>::value) // TODO: Figure out how to use these noexcept conditions properly
	{
		return (*this = std::move(f));
	}

	operator NodeType&() { return data; }
	operator NodeType() const { return data; }

};

// A sorted vector that uses 
// binary search when finding elements
template<class Traits>
class FlatTree
{
public:
	using MyBase = FlatTree<Traits>;
	using BaseTypes = OrderedTypes<Traits, FlatTreeNode>;

	using Node			  = typename BaseTypes::Node;
	using NodePtr		  = typename BaseTypes::NodePtr;
	//using NodeEqual		  = typename Traits::node_equal;
	//using NodeType		  = typename BaseTypes::node_type;
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

	using iterator = typename Storage::iterator;
	using const_iterator = typename Storage::const_iterator;

	iterator begin() noexcept { return data.begin(); }
	iterator end() noexcept { return data.end(); }
	const_iterator begin() const noexcept { return data.begin(); }
	const_iterator end() const noexcept { return data.end(); }
	const_iterator cbegin() const noexcept { return data.cbegin(); }
	const_iterator cend() const noexcept { return data.cend(); }

private:
	NodeAl nodeAl;

	Storage data;

	size_type upperBound(const key_type &k)
	{
		size_type size = data.size() - 1;
		size_type low = 0;

		while (size > 0) 
		{
			size_type half = size / 2;
			size_type ohalf = size - half;
			size_type probe = low + half;
			size_type olow = low + ohalf;
			size = half;

			low = key_compare()(get_key()(data[probe]), k) ? olow : low;
		}

		return low;
	}

public:

	void reserve(size_type newCap)
	{
		data.reserve(newCap);
	}

	template<class... Args>
	void emplace(Args&& ...args)
	{
		++data.MySize;
		if (data.capacity() <= data.size() - 1)
			data.grow(); // TODO: data.OldLast on resize appears to be placed one farther than it should be

		// Temporary construction
		// While we find it a place to sit
		Node n(std::forward<Args>(args)...);

		size_type idx = upperBound(get_key()(n));

		data.constructInPlace(idx, std::move(n));
	}
};