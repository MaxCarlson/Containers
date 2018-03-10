
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

inline void timer(bool first)
{
	using namespace std::chrono;

	static std::chrono::time_point<std::chrono::steady_clock> start;

	if (first)
	{
		start = high_resolution_clock::now();
		return;
	}

	std::cout << duration_cast<duration<double>>(high_resolution_clock::now() - start).count();
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

	timer(true);
	for (int i = 0; i < 20000; ++i)
	{
		//tree.insert(i); // Worst case for balance
		tree.insert(rand());
		tree.find(rand()); // half as slow as unordered_set, 50% faster than set

	}

	int idx = 0;
	for (auto i : tree)
	{
		std::cout << i << " ";
	}

	timer(false);
	int a = 5;
}

int main()
{ 
	using namespace std::chrono;
	using std::chrono::high_resolution_clock;
	auto start = high_resolution_clock::now();

	std::vector<int> fff = { 4, 4, 4, 1 };

	auto b = fff.begin();

	std::set<int> ss;
	auto sb = ss.begin();
	

	//testVector()
	testBST();

	return 0;
}