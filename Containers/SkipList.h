#pragma once
#include "OrderedStructuresHelpers.h"

template<class Traits>
struct SkipNode
{

};



template<class Traits>
class SkipList
{
	using MyBase = SkipList<Traits>;
	using BaseTypes = OrderedTypes<Traits, RedBlackNode>;

};