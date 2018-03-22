#pragma once
#include "RbTree.h"



template<class Key, class Type, 
	class Compare = std::less<Key>, 
	class Allocator = std::allocator<std::pair<const Key, Type>>, 
	class MyBase = RedBlackTree<Key, Compare, Allocator>> // This is awkward having this as last param in template
class Map : public MyBase
{

};