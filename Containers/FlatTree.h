#pragma once
#include "OrderedStructuresHelpers.h"
#include <utility>


template<class NodeType>
struct FlatTreeNode
{
	NodeType data;
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

private:

};