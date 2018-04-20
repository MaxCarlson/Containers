#include "stdafx.h"
#include "../Containers/FlatTree.h"
#include "../Containers/Set.h"
#include "../Containers/Map.h"
#include <set>
#include <random>
#include <utility>
#include <tuple>
//#include <unordered_set>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestFlatTreeN
{
	TEST_CLASS(TestFlatTree)
	{
		const int num = 10000;

		TEST_METHOD(FlatTree__EmplaceAndIterate)
		{
			std::set<int> test;
			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;

			for (int i = 0; i < num; ++i)
			{
				const int v = rand();

				auto it = test.emplace(v);
				auto sit = set.emplace(v);
				auto mit = map.emplace(v, v);

				// Check the values
				Assert::AreEqual(*it.first, *sit.first, L"Set Error");
				Assert::AreEqual(*it.first, mit.first->first, L"Set Error");
				// Check the bools
				Assert::AreEqual(it.second, sit.second, L"Set Error");
				Assert::AreEqual(it.second, mit.second, L"Set Error");
			}

			int idx = 0;
			auto sit = set.begin();
			auto mit = map.begin();
			for (auto it = test.begin(); it != test.end(); ++it, ++sit, ++mit)
			{
				Assert::AreEqual(*it, *sit, L"Set Error");
				Assert::AreEqual(*it, mit->first, L"Map Error");
			}
		}

		TEST_METHOD(FlatTree__Find)
		{
			std::set<int> tset;
			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;

			for (int i = 0; i < num; ++i)
			{
				set.emplace(i);
				map.emplace(i, i);
				tset.emplace(i);
			}
			for (int i = 0; i < num; ++i)
			{
				auto it = tset.find(i);
				auto sit = set.find(i);
				auto mit = map.find(i);

				Assert::AreEqual(*it, *sit, L"Set Find Error");
				Assert::AreEqual(*it, mit->first, L"Map Find Error");
			}
		}

		TEST_METHOD(FlatTree__EmplaceHint)
		{
			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;
			std::set<int> test;

			int i = 0;
			for (i; i < 150; ++i)
			{
				test.emplace(i);
				set.emplace_hint(set.end(), i);
				map.emplace_hint(map.end(), i, i);
			}

			for (i; i < 300; ++i)
			{
				test.emplace(i);
				set.emplace_hint(set.begin(), i);
				map.emplace_hint(map.begin(), i, i);
			}

			for (i; i < num; ++i)
			{
				int r = rand();
				int low = r - 1;
				int high = r + 1;

				test.emplace(r);
				test.emplace(r - 1);
				test.emplace(r + 1);

				auto sit = set.emplace_hint(set.begin(), r);
				set.emplace_hint(sit, low);
				set.emplace_hint(set.end(), high);

				auto mit = map.emplace_hint(map.begin(), r, r);
				map.emplace_hint(mit, low, low);
				map.emplace_hint(map.end(), high, high);
			}

			auto sit = set.begin();
			auto mit = map.begin();
			for (auto it = test.begin(); it != test.end(); ++it, ++sit, ++mit)
			{
				Assert::AreEqual(*it, *sit);
				Assert::AreEqual(*it, mit->first);
			}
		}

		TEST_METHOD(FlatTree__UpperLowerBound)
		{
			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;
			std::set<int> test;

			for (int i = 0; i < num; ++i)
			{
				set.emplace(i);
				map.emplace(i, i);
				test.emplace(i);
			}
			for (int i = 0; i < num; ++i)
			{
				int r = rand() % num;

				auto sit = set.upper_bound(r);
				auto mit = map.upper_bound(r);
				auto it = test.upper_bound(r);
				Assert::AreEqual(*it, *sit);
				Assert::AreEqual(*it, mit->first);

				sit = set.lower_bound(r);
				mit = map.lower_bound(r);
				it = test.lower_bound(r);
				Assert::AreEqual(*it, *sit);
				Assert::AreEqual(*it, mit->first);
			}
		}

		TEST_METHOD(FlatTree__AssignmentOp)
		{
			std::set<size_t> test;
			Set<size_t, FlatTree> sets;
			Set<int, FlatTree> seti;
			Map<int, size_t, FlatTree> maps;
			Map<int, int, FlatTree> mapi;

			for (int i = 0; i < num; ++i)
			{
				test.emplace(i);
				sets.emplace(i);
				maps.emplace(i, i); 
			}

			seti = sets;
		}
	};
}
