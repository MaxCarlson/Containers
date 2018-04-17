#pragma once
#include "helpers.h"

// Wrapper for types needed by tree and iterators
template<class Traits, template<class> class NodeType>
struct OrderedTypes
{
	using Alloc = typename Traits::allocator_type;
	using key_type = typename Traits::key_type;
	using key_compare = typename Traits::key_compare;
	using node_type = typename Traits::node_type;

	using Node = NodeType<node_type>;

	using NodeAl = RebindAllocator<Alloc, Node>;
	using NodeAlTraits = std::allocator_traits<NodeAl>;
	using AllocTraits = std::allocator_traits<Alloc>;

	using NodePtr = typename NodeAlTraits::pointer;
	using difference_type = typename AllocTraits::difference_type;
	using value_type = typename AllocTraits::value_type;
	using pointer = typename AllocTraits::pointer;
	using const_pointer = typename AllocTraits::const_pointer;
	using reference = value_type & ;
	using const_reference = const value_type&;
};

template<class Traits, class NodeType> // TODO: Combine the majority of these
struct OrderedTypesNoNodeWrapper
{
	using Alloc = typename Traits::allocator_type;
	using key_type = typename Traits::key_type;
	using key_compare = typename Traits::key_compare;
	//using node_type = typename Traits::node_type;

	using Node = NodeType;

	using NodeAl = RebindAllocator<Alloc, Node>;
	using NodeAlTraits = std::allocator_traits<NodeAl>;
	using AllocTraits = std::allocator_traits<Alloc>;

	using NodePtr = typename NodeAlTraits::pointer;
	using difference_type = typename AllocTraits::difference_type;
	using value_type = typename AllocTraits::value_type;
	using pointer = typename AllocTraits::pointer;
	using const_pointer = typename AllocTraits::const_pointer;
	using reference = value_type & ;
	using const_reference = const value_type&;
};