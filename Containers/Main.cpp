
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
	BinarySearchTree<int> tree;

	constexpr auto count = 350;

	timer(true);
	for (int i = 0; i < count; ++i)
	{
		//tree.insert(i); // Worst case for balance
		tree.insert(rand());
	}
	/*
	for (int i = 0; i < count; ++i)
	{
		auto f = tree.find(i);

		if (f)
			tree.insert(*f);
		else
			tree.insert(rand());
	}
	*/
	///*
	int idx = 0;
	for (auto i : tree)
	{
		std::cout << i << " ";
		++idx;
	}
	//*/

	timer(false);
	int a = 5;
}

int main()
{ 
	//testVector()
	testBST();

	return 0;
}