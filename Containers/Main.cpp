#include "BinaryTree.h"
#include "RedBlackTree.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
#include "RobinHood.h"
#include "Map.h"
#include "Set.h"
#include "SmallVec.h"
#include "FlatTree.h"
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
	//constexpr long long num = 5000; 
	constexpr long long num = 1000000; // For non-optimized
	//constexpr long long num = 40000000; // For release
	using Key = int;
	using Value = int;
	std::uniform_int_distribution<int> distri(0, num);

	//{
	//std::unordered_set<Key> set;
	//std::unordered_map<Key, Value> set;
	UnorderedSet<Key, RobinhoodHash> robin;
	UnorderedMap<Key, Value, RobinhoodHash> rm;
	UnorderedSet<Key> set;
	//UnorderedMap<Key, Value, OpenAddressLT, HashTT, HashEqual> set;
	//UnorderedSet<Key> set;

	timer<Key>(true);

	//std::unordered_map<Key, Value> inserted; inserted.max_load_factor(0.0f);

	int f = sizeof(RobinhoodNode<std::pair<Key, Value>>);

	for (auto i = 0; i < num; ++i)
	{
		//auto r = distri(defRand);

		//std::cout << i << " ";
		if (i == 13)
			int a = 5;

		robin.emplace(i);
	}


	UnorderedMap<Key, Value> newRobin;

	for (auto i = 0; i < 10000; ++i)
		newRobin.emplace(i, i);
	for (auto it = newRobin.begin(); it != newRobin.end(); )
	{
		it = newRobin.erase(it);
	}

	for (auto it = newRobin.begin(); it != newRobin.end(); ) // Weird bug, very very long iteration time when table is "Empty"
	{
		it = newRobin.erase(it);
	}


//	robin = newRobin;

	int b = 5;
	for (auto it = robin.begin(); it != robin.end();)
		it = robin.erase(it);
	//}

	timer<Key>(false);

	int a = 5;
}

struct OBJ
{

	OBJ(int z) : f(z), d(z) {}
	int f;
	int d;

	//OBJ(OBJ&& o) = delete;
	/*
	OBJ& operator=(const OBJ& o)
	{
		f = o.f;
		d = o.d;
		return *this;
	}
	*/
};
#include <utility>
template<class T>
void isIntegralP(T t)
{
	using t1 = std::remove_cv_t<decltype(*t)>;
	typedef typename std::remove_reference<decltype(*std::declval<T>())>::type F;

	std::cout << typeid(t1).name() << '\n';   // Prints out int
	std::cout << std::is_integral<t1>::value << '\n'; // Prints false(0)
	std::cout << typeid(F).name() << '\n';   // Prints out int
	std::cout << std::is_integral<F>::value << '\n'; // Prints false(0)
}

void testSmallVec()
{
	static constexpr int num = 250000000;
	using Key = int;

	SmallVec<Key, 1> vec;
	SmallVec<size_t, 13> vec13;

	SmallVec<int, 15> vec15;
	SmallVec<int, 80> vec80;
	SmallVec<int, 300> vec300;
	SmallVec<int, 1000> vec1000;

	timer<Key>(true);

	int base = 0;
	int* p = &base;
	isIntegralP(p);
	uncheckedMove(p, p, p);

	for (int i = 0; i < num; ++i)
	{
		auto it = vec.emplace_back(i);

		if(i % 100 == 0 && i != 0) 
		vec.erase(vec.begin());

		//vec15.emplace_back(i);
		//vec80.emplace_back(i);
		//vec300.emplace_back(i);
		//vec1000.emplace_back(i);
	}

	timer<Key>(false);


	//vec13 = vec;

}

void printOutSmallVec(SmallVec<int, 1> &v)
{
	for (auto i : v)
		std::cout << i << " ";

	std::cout << "\n";
}

void testFlat()
{
	//static constexpr int num = 10000;
	static constexpr int num = 100000000;

	using Key = int;

	Set<Key, FlatTree> set;
	Map<Key, Key, FlatTree> map;

	timer<int>(true);

	for (int i = 0; i < num; ++i)
	{
		auto it = set.emplace(i);
		//map.emplace(i, i);


		set.emplace_hint(set.end(), i);
	}

	timer<int>(false);

	int a = 5;
	//std::vector<int> ff; ff.clear();
}

// Things to implement
// Flat-Maps
// B-Tree
// AVL-Tree
// Splay-Tree
// Trie
// Hash tables
// Skip List
// Deque
// bloom-filter
// Pool allocated lists
// lock-free stuff
// Pool allocate other stuff as well

// vector + uset/umap fast lookups fast traversal 
// store and iterate through vector when it's small
// allocate uset/map once larger for find ops
// for erase either do a linear erase or store vec idx's in the map nodes

int main()
{ 
//	for(int i = 0; i < 100; ++i)
//		increaseCapacity();

	
	//testMap();
	//testHash();
	testSmallVec();
	//testFlat();
	return 0;
}
