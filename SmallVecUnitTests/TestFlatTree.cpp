#include "stdafx.h"
#include "../Containers/FlatTree.h"
#include "../Containers/Set.h"
#include "../Containers/Map.h"
#include <set>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FlatTreeUnitTests
{
	TEST_CLASS(TestFlatTree)
	{
		const int num = 10000;

		TEST_METHOD(FlatTree__Emplace)
		{
			std::vector<int> test;

			Set<int, FlatTree> set;
			Map<int, int, FlatTree> map;

			for (int i = 0; i < num; ++i)
			{
				const int v = rand();

				test.emplace_back(v);
				set.emplace(v);
				map.emplace(v, v);
			}

			std::sort(test.begin(), test.end(), test.end());

			int idx = 0;

			auto sit = set.begin();
			auto mit = map.begin();
			for (auto it = test.begin(); it != test.end(); ++it, ++sit, ++mit)
			{
				Assert::AreEqual(*it, *sit); 
			}
		}






	};
}
