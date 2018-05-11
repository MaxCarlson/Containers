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
	static constexpr auto num = 2500000000;
	//static constexpr int num = 2500000;

	using Key = int;

	SmallVec<Key, 1000> vec;
	//std::vector<int> test;

	vec.reserve(1000);
	timer<Key>(true);

	for (long i = 0; i < num; ++i)
	{
		auto it = vec.emplace_back(i);

		if (vec.size() >= 100)
		{
			vec.fast_erase(vec.begin(), vec.begin() + 5);
		}
	}

	timer<Key>(false);

	//vec13 = vec;
	int a = 5;
}

#include <boost\container\flat_set.hpp>

void testFlat()
{
	//static constexpr int num = 10000;
	static constexpr int num = 50000000;

	using Key = int;

	Set<Key, FlatTree> test;
	//Set<Key, RedBlackTree> test;
	//std::set<int> test;
	//boost::container::flat_set<int> test;

	//UnorderedSet<Key, RobinhoodHash> test;

	std::uniform_int_distribution<int> rng(-num, num);

	timer<int>(true);

	for (int i = 0; i < num; ++i)
	{
		const int r = rng(defRand);
		test.emplace(i); 
	}


	timer<int>(false);

	int a = 5;
	int b = a + 5;
	std::cout << b;
	//std::vector<int> ff; ff.clear();
}

// Things to implement
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

int main()
{ 
	testSmallVec();
	return 0;
}
