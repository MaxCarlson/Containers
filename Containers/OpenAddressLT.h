#pragma once
#include <memory>

// Open address liner probeing table

// Wrapper for types needed by Hashtable
template<class Traits>
struct TreeTypes
{
	using Alloc = typename Traits::allocator_type;
	using AllocTraits = std::allocator_traits<Alloc>;

	using key_type  = typename Traits::key_type;

	using difference_type = typename AllocTraits::difference_type;
	using value_type	  = typename AllocTraits::value_type;
	using pointer		  = typename AllocTraits::pointer;
	using const_pointer   = typename AllocTraits::const_pointer;
	using reference		  = value_type & ;
	using const_reference = const value_type&;
};

template<class Traits>
class OpenAddressLT
{
	using MyBase = OpenAddressLT<Traits>;
	using BaseTypes = TreeTypes<Traits>;

	using key_equal   = typename Traits::key_equal;
	using value_equal = typename Traits::value_equal;

};