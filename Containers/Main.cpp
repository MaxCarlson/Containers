
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

template<class C>
inline void printCVector(Vector<C> &v)
{
	for (auto it = v.begin(); it != v.end(); ++it)
		std::cout << *it << " ";

	std::cout << "\n";
}

inline void timer(bool startNow)
{
	using namespace std::chrono;

	static std::chrono::time_point<std::chrono::steady_clock> start;

	if (startNow)
	{
		start = high_resolution_clock::now();
		return;
	}

	std::cout << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "\n";
}

void testMap()
{
	constexpr auto num = 5050000;
	using Key = int;
	using Value = int;
	std::uniform_int_distribution<int> distri(0, num);

	//Map<Key, Value, RedBlackTree> map;
	std::map<Key, Value> map;

	timer(true);

	//for (int times = 0; times < num; ++times){
	
	for (auto i = 0; i < num; ++i)
	{
		const int x = distri(defRand);

		//smap.emplace(x);
		map.emplace(x, i);
	}

	timer(false);
	timer(true);

	for (auto it = map.begin(); it != map.end();)
	{
		it = map.erase(it);
	}

	timer(false);
	
	//}

	int a = 5;
}

void testHash()
{
	constexpr auto num = 500000;
	using Key = int;
	std::uniform_int_distribution<Key> distri(0, num);

	std::unordered_set<Key> set;
	//UnorderedSet<Key> set;

	timer(true);

	for (int i = 0; i < num; ++i)
	{
		auto r = distri(defRand);
		auto it = set.emplace(r);

		auto itf = set.find(r);
	}

	timer(false);

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
