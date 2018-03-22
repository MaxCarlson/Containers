
#include "Vector.h"
#include "BinaryTree.h"
#include "RbTree.h"
#include "TemplateTesting.h"
#include <vector>
#include <iostream>
#include <map>
#include <random>
#include <chrono>

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
#include "Map.h"
void testMap()
{
	constexpr int num = 100;
	using Key = int;
	using Value = int;

	//std::map<Key, Value> stdmap;
	Map<Key, Value> map;

	for (int i = 0; i < num; ++i)
	{
		map.emplace(i, i);
	}

	int a = 5;
}

// Things to implement
// Hash table
// Skip List
// Deque?

int main()
{ 
	testMap();

	return 0;
}