#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include "../Containers/HashtableHelpers.h"
#include "../Containers/helpers.h"

#include "../Containers/RobinHood.h"
#include "../Containers/OpenAddressLinear.h"
#include "../Containers/UnorderedMap.h"
#include "../Containers/UnorderedSet.h"


namespace UnitTestHashTables
{
	TEST_CLASS(TestHashTables)
	{
	public:

		static const size_t num = 100000;

		template<class Arg>
		void fillContainers(Arg& arg)
		{
			for (auto i = 0; i < num; ++i)
				if constexpr(std::is_same<typename Arg::iterator, typename Arg::const_iterator>::value)
					arg.emplace(i);
				else
					arg.emplace(i, i);
		}

		template<class Arg, class... Args>
		void fillContainers(Arg&& arg, Args&&... args)
		{
			fillContainers(arg);
			fillContainers(args...);
		}

		TEST_METHOD(HashTables__Insert)
		{
			UnorderedSet<int, OpenAddLinear> openSet;
			UnorderedMap<int, int, OpenAddLinear> openMap;
			UnorderedSet<int, RobinhoodHash> robinSet;
			UnorderedMap<int, int, RobinhoodHash> robinMap;

			for (int i = 0; i < num; ++i)
			{
				auto os = openSet.emplace(i); // Open address set/map
				auto om = openMap.emplace(i, i);
				auto rs = robinSet.emplace(i); // Robinhood set/map
				auto rm = robinMap.emplace(i, i);

				Assert::AreEqual(true, os.second, L"Open Insert Failed");
				Assert::AreEqual(true, om.second, L"Open Insert Failed");
				Assert::AreEqual(true, rs.second, L"Robin Insert Failed");
				Assert::AreEqual(true, rm.second, L"Robin Insert Failed");

			}

			Assert::AreEqual(num, openSet.size(), L"OpenSet  inserted incorrect number of elements");
			Assert::AreEqual(num, openMap.size(), L"OpenMap  inserted incorrect number of elements");
			Assert::AreEqual(num, robinSet.size(), L"RobinSet inserted incorrect number of elements");
			Assert::AreEqual(num, robinMap.size(), L"RobinMap inserted incorrect number of elements");
		}

		TEST_METHOD(HashTables__Find)
		{
			UnorderedSet<int, OpenAddLinear> openSet;
			UnorderedSet<int, RobinhoodHash> robinSet;
			UnorderedMap<int, int, OpenAddLinear> openMap;
			UnorderedMap<int, int, RobinhoodHash> robinMap;

			fillContainers(openSet, openMap, robinSet, robinMap);

			for (int i = 0; i < num; ++i)
			{
				auto itos = openSet.find(i);
				auto itom = openMap.find(i);
				Assert::AreEqual(*itos, i, L"Openet inserted key is wrong");
				Assert::AreEqual(itom->first, i, L"OpenMap inserted key is wrong");
				Assert::AreEqual(itom->second, i, L"OpenMap inserted value is wrong");

				auto itrs = robinSet.find(i);
				auto itrm = robinMap.find(i);
				Assert::AreEqual(*itrs, i, L"RobinSet inserted key is wrong");
				Assert::AreEqual(itrm->first, i, L"RobinMap inserted key is wrong");
				Assert::AreEqual(itrm->second, i, L"RobinMap inserted value is wrong");
			}
		}

		TEST_METHOD(HashTables__Erase_Its)
		{
			UnorderedSet<int, OpenAddLinear> openSet;
			UnorderedSet<int, RobinhoodHash> robinSet;
			UnorderedMap<int, int, OpenAddLinear> openMap;
			UnorderedMap<int, int, RobinhoodHash> robinMap;

			fillContainers(openSet, openMap, robinSet, robinMap);

			for (auto it = openSet.begin(); it != openSet.end(); )
				it = openSet.erase(it);
			Assert::AreEqual(size_t(0), openSet.size(), L"OpenSet failed to delete elements");

			for (auto it = openMap.begin(); it != openMap.end(); )
				it = openMap.erase(it);
			Assert::AreEqual(size_t(0), openMap.size(), L"OpenMap failed to delete elements");

			for (auto it = robinSet.begin(); it != robinSet.end(); )
				it = robinSet.erase(it);
			Assert::AreEqual(size_t(0), robinSet.size(), L"RobinSet failed to delete elements");

			for (auto it = robinMap.begin(); it != robinMap.end(); )
				it = robinMap.erase(it);
			Assert::AreEqual(size_t(0), robinMap.size(), L"RobinMap failed to delete elements");
		}
	};
}