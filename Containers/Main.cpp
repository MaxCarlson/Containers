
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

void testVector()
{
	timer(true);

	//Vector<int> test;
	std::vector<int> test;
	for (int i = 0; i < 100000000; ++i)
	{
		//Larger r;
		//test.push_back(r);

		//auto t = rand();
		test.emplace_back(rand());
		test.push_back(1);
		//auto dd = test[i];

		//printCVector<int>(test);
	}
	//printCVector<int>(test);
	timer(false);
}

#include <set>
#include <unordered_set>

void testBST()
{
	//std::set<int> tree;
	//std::unordered_set<int> tree;
	BinaryTree<int> tree;

	tree.insert(0);
	tree.insert(1);
	tree.insert(2);
	//tree.insert(2);
	//tree.insert(4);

	//tree.pbRight();
	tree.pbLeft();
	//tree.pbRight();
	//tree.pbRight();
	//tree.pbRight();
	//tree.pbRight();
	
	
	timer(false);
	int a = 5;
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

void testForwarding()
{
	RedBlackTree<TestForward, CompareForward> tree;

	for (int i = 0; i < 1000; ++i)
	{
		auto i1 = rand();
		auto i2 = rand();
		auto i3 = rand();

		tree.emplace( i1, i2, i3 );
	}
}

void testRbTree()
{
	constexpr auto count = 59999;
	using Type = int;

	std::set<Type> t;
	RedBlackTree<Type> tree;
	//BinaryTree<Type> tree;

	//auto al = std::allocator<Type>();

	timer(true);

	for (int i = 0; i < count; ++i)
	{
		//tree.rotate<RedBlackTree<int>::Direction::RIGHT>(nullptr);
		tree.emplace(i);
		t.emplace(i);
	//	tree.emplace(rand());

		auto tt = tree.find(i);

		//tree.erase(i);
		
		//t.emplace(i);
		//t.find(i);
	}

	const auto itt = tree.begin();

	for (auto it = tree.rbegin(); it != tree.rend(); ++it)  ;
		//std::cout << *it << " ";

	for (auto it = tree.cbegin(); it != tree.cend();)
	{
		it = tree.erase(it);
		++it;
	}

	
	for (int i = 0; i < count * 2; ++i)
	{
		Type r = rand() % count;
		//t.erase(r);
		tree.erase(r);
	}

	timer(false);
	int a = 5;
}

#include <map>
#include "Map.h"
void testMap()
{

	Map<size_t, int> m;
	std::map<size_t, int> stdMap;
	
}

// Things to implement
// Hash table
// Skip List
// Deque?

int main()
{ 
	test.test();

	//testVector()
	//testBST();
	testRbTree();
	testForwarding();
	//testMap();

	return 0;
}