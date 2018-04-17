#include "stdafx.h"
#include "../Containers/FlatTree.h"
#include "../Containers/Set.h"
#include "../Containers/Map.h"
#include <set>
#include <random>
#include <utility>
#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestFlatTree
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
			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;

			for (int i = 0; i < num; ++i)
			{
				set.emplace(i);
				map.emplace(i, i);
			}

			for (int i = 0; i < num; ++i)
			{
				auto sit = set.find(i);
				auto mit = map.find(i);

				Assert::AreEqual(true, sit != set.end(), L"Set Find Error");
				Assert::AreEqual(true, mit != map.end(), L"Map Find Error");
			}
		}
	};
}
