#include "BinaryTree.h"
#include "RedBlackTree.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
#include "RobinHood.h"
#include "Map.h"
#include "Set.h"
#include "SmallVec.h"
#include "FlatTree.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <map>
#include <random>
#include <chrono>
#include <set>
#include <random>

std::default_random_engine defRand;

struct OBJ
{

	OBJ(int z) : f(z), d(z) {}
	int f;
	int d;

	//OBJ(OBJ&& o) = delete;
	/*
	OBJ& operator=(const OBJ& o)
	{
		f = o.f;
		d = o.d;
		return *this;
	}
	*/
};

void testSmallVec()
{
	static constexpr int num = 250000000;
	//static constexpr int num = 2500000;

	using Key = int;

	SmallVec<Key, 1000> vec;
	//std::vector<int> test;


	timer<Key>(true);

	for (int i = 0; i < num; ++i)
	{
		auto it = vec.emplace_back(i);

		if (vec.size() >= 999)
			vec.clear();
	}

	timer<Key>(false);

	//vec13 = vec;
	int a = 5;
}

void testFlat()
{
	static constexpr int num = 10000;
	//static constexpr int num = 10000000;

	using Key = int;

	Set<int16_t, FlatTree> sets;
	Set<Key, FlatTree> set;
	Map<Key, Key, FlatTree> map;

	timer<int>(true);

	for (int i = 0; i < num; ++i)
	{
		auto it = set.emplace(i);
		//map.emplace(i, i);


		set.emplace_hint(set.end(), i);
	}

	set = sets;

	timer<int>(false);

	int a = 5;
	//std::vector<int> ff; ff.clear();
}

// Things to implement
// Flat-Maps
// B-Tree
// AVL-Tree
// Splay-Tree
// Trie
// Hash tables
// Skip List
// Deque
// bloom-filter
// Pool allocated lists
// lock-free stuff
// Pool allocate other stuff as well

// vector + uset/umap fast lookups fast traversal 
// store and iterate through vector when it's small
// allocate uset/map once larger for find ops
// for erase either do a linear erase or store vec idx's in the map nodes

int main()
{ 
//	for(int i = 0; i < 100; ++i)
//		increaseCapacity();

	
	//testMap();
	//testHash();
	testSmallVec();
	//testFlat();
	return 0;
}
