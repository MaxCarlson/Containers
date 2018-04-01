
#include "Vector.h"
#include "BinaryTree.h"
#include "RedBlackTree.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
#include "RobinHood.h"
#include "Map.h"
#include "Set.h"
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


void testMap()
{
	constexpr auto num = 5050000;
	using Key = int;
	using Value = int;
	std::uniform_int_distribution<int> distri(0, num);

	//Map<Key, Value, RedBlackTree> map;
	std::map<Key, Value> map;


	//for (int times = 0; times < num; ++times){
	
	for (auto i = 0; i < num; ++i)
	{
		const int x = distri(defRand);

		//smap.emplace(x);
		map.emplace(x, i);
	}

	for (auto it = map.begin(); it != map.end();)
	{
		it = map.erase(it);
	}

	
	//}

	int a = 5;
}


struct TT
{
	TT(int aa)
	{
		a = aa;
	}

	int a;
};

struct HashTT
{
	size_t operator()(const TT& t) const
	{
		return hasher(t.a);
	}

	std::hash<int> hasher;
};

struct HashEqual
{
	bool operator()(const TT& t, const TT& tt) const
	{
		return t.a == tt.a;
	}
};

void testHash()
{
	constexpr long long num = 40000000;
	using Key = int;
	using Value = int;
	std::uniform_int_distribution<int> distri(0, num);

	//std::unordered_set<Key> set;
	//std::unordered_map<Key, Value> set;
	UnorderedSet<Key, OpenAddLinear> set;
	UnorderedMap<Key, Value, RobinhoodHash> robin;
	//UnorderedSet<Key> set;
	//UnorderedMap<Key, Value, OpenAddressLT, HashTT, HashEqual> set;
	//UnorderedSet<Key> set;

	timer<Key>(true);

	//std::unordered_map<Key, Value> inserted; inserted.max_load_factor(0.0f);

	int iii = sizeof(short);
	int ii = sizeof(int16_t);
	int f = sizeof(RobinhoodNode<int>);

	for (auto i = 0; i < num; ++i)
	{
		//auto r = distri(defRand);

		robin.emplace(i, i);

		//set.emplace(i);

	//	auto f = set.find(i);

		//auto eq = set.equal_range(r);

	//	inserted.emplace(r, r - 5);
		//int a = 5;
	}

	//for (auto it = set.begin(); it != set.end();)
	//	it = set.erase(it);
	
	timer<Key>(false);

	int a = 5;
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
	

	//testMap();
	testHash();
	return 0;
}
