
#include "Vector.h"
#include "BinaryTree.h"
#include "RbTree.h"
#include "TemplateTesting.h"
#include "UnorderedSet.h"
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

void testHash()
{
	constexpr auto num = 100000;
	using Key = int;
	std::uniform_int_distribution<Key> distri(0, num * 10000);

	//std::unordered_set<Key> set;
	UnorderedSet<Key> set;

	timer<Key>(true);

	for (int i = 0; i < num; ++i)
	{
		auto r = distri(defRand);
		auto it = set.emplace(r);

		auto itf = set.find(r);
	}

	timer<Key>(false);

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
