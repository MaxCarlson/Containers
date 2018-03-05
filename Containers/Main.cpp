
#include "vectorImpl.h"
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

struct Larger
{
	Larger()
	{
		auto i = rand();
		apples.resize(1000, i);
	}
	std::vector<uint64_t> apples;
};


inline double timer()
{

}

void testVector()
{
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
}

void testBST()
{
	BinarySearchTree<int> tree;

	for (int i = 2; i < 100; ++i)
	{
		tree.insert(i);

		auto* t = tree.find(i);
	}
}

int main()
{ 
	using namespace std::chrono;
	using std::chrono::high_resolution_clock;
	auto start = high_resolution_clock::now();

	//testVector()
	testBST();

	auto end = high_resolution_clock::now();

	auto t = duration_cast<duration<double>>(end - start).count();

	std::cout << t;

	return 0;
}