
#include "Vector.h"
#include "BinaryTree.h"
#include "RbTree.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
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
	constexpr long long num = 19500000;
	using Key = uint64_t;
	using Value = int;
	std::uniform_int_distribution<uint64_t> distri(0, num * 1000);

	//std::unordered_set<Key> set;
	//std::unordered_map<Key, Value> set;
	UnorderedMap<Key, Value> set;
	//UnorderedMap<Key, Value, OpenAddressLT, HashTT, HashEqual> set;
	//UnorderedSet<Key> set;

	timer<Key>(true);

	//std::unordered_map<Key, Value> inserted;

	for (auto i = 0; i < num; ++i)
	{
		auto r = distri(defRand);

		auto it = set.emplace(r, r - 5);

		//auto itf = set.find(r);

		set.erase(r);

		//auto eq = set.equal_range(r);

	//	inserted.emplace(r, r - 5);
		int a = 5;
	}
	
//	int numNotFound = 0;
//	for (const auto i : inserted)
//		if (set.find(i.first) == set.end() || set.find(i.first)->second != i.second) // Not good! This triggers bp
//			++numNotFound;

	timer<Key>(false);
//	std::cout << "Num not found: " << numNotFound;

	int a = 5;
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

int main()
{ 
	//testMap();
	testHash();

	return 0;
}
