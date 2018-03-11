
#include "Vector.h"
#include "BinarySearchTree.h"

#include <vector>
#include <iostream>

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

	std::cout << "\n" << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "\n";
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
	BinarySearchTree<uint64_t> tree;

	constexpr auto count = 9500000;

	timer(true);
	///*
	for (int i = 0; i < count; ++i)
	{
		//tree.insert(i); // Worst case for balance
		tree.insert(rand());
		tree.erase(rand());
	}
	
	for (int i = 0; i < count; ++i)
	{
		tree.find(rand());
	}
	//*/

	/*
	tree.insert(3);
	tree.insert(1);
	tree.insert(0);
	tree.insert(2);
	tree.insert(4);

	tree.pbRight();
	tree.pbLeft();
	tree.pbRight();
	tree.pbRight();
	tree.pbRight();
	tree.pbRight();
	*/
	
	timer(false);
	int a = 5;
}
#include <map>
int main()
{ 
	//testVector()
	testBST();

	return 0;
}