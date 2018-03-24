
#include "Vector.h"
#include "BinaryTree.h"
#include "RbTree.h"
#include "TemplateTesting.h"
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

struct TestForward
{
	int a;
	int b;
	int c;

	bool operator==(const TestForward &t)
	{
		return a == t.a;
	}
};

struct CompareForward
{
	bool operator()(const TestForward & t, const TestForward &d)
	{
		return t.a < d.a;
	}
};

void testMap()
{
	constexpr int num = 9000000;
	using Key = int;
	using Value = int;

	Map<Key, Value, RedBlackTree> map;
	//std::map<Key, Value> map;

	for (int i = 0; i < num; ++i)
	{
		map.emplace(rand(), rand());
	}

	timer(true);

	for (auto it = map.begin(); it != map.end();)
	{
		it = map.erase(it);
	}

	timer((false));

	int a = 5;
}

void testHash()
{
	std::unordered_set<int> stdset;
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
	testMap();


	return 0;
}
