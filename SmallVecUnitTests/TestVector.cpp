#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Containers/SmallVec.h"
#include <utility>
#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SmallVecUnitTests
{		
	TEST_CLASS(TestVector)
	{
	public:
		static constexpr int count = 100000;
		static constexpr size_t sc = count;

		template<class... Args>
		void fillVectors(Args&& ...args)
		{
			constexpr int ccc = sizeof...(Args);
			auto tt = std::make_tuple( args... );

			for (int i = 0; i < count; ++i)
				for (int j = 0; j < std::tuple_size_v<std::tuple<Args...>>; ++j)
					std::get<std::index_sequence_for<Args...>>(tt).emplace(i);

		}

		TEST_METHOD(SmallVector__TestInsert)
		{		
			SmallVec<int, 1> vec;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 300> vec300;
			SmallVec<int, 1000> vec1000;

			fillVectors(vec, vec15, vec80);

			for (int i = 0; i < count; ++i)
			{
				vec.emplace_back(i);
				vec15.emplace_back(i);
				vec80.emplace_back(i);
				vec300.emplace_back(i);
				vec1000.emplace_back(i);

				Assert::AreEqual(i, vec[i]);
				Assert::AreEqual(i, vec15[i]);
				Assert::AreEqual(i, vec80[i]);
				Assert::AreEqual(i, vec300[i]);
				Assert::AreEqual(i, vec1000[i]);
			}
			Assert::AreEqual(sc, vec.size());
			Assert::AreEqual(sc, vec15.size());
			Assert::AreEqual(sc, vec80.size());
			Assert::AreEqual(sc, vec300.size());
			Assert::AreEqual(sc, vec1000.size());
		}

		TEST_METHOD(SmallVector__TestPopBack)
		{
			SmallVec<int, 1> vec;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 300> vec300;
			SmallVec<int, 1000> vec1000;

			for (int i = 0; i < count; ++i)
			{
				vec.emplace_back(i);
				vec15.emplace_back(i);
				vec80.emplace_back(i);
				vec300.emplace_back(i);
				vec1000.emplace_back(i);
			}

			for (int i = count - 1; i >= 0; --i)
			{
				vec.pop_back();
				vec15.pop_back();
				vec80.pop_back();
				vec300.pop_back();
				vec1000.pop_back();
			}

		}

		TEST_METHOD(SmallVector__Iterating)
		{
			SmallVec<int, 1> vec;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 300> vec300;
			SmallVec<int, 1000> vec1000;

			for (int i = 0; i < count; ++i)
			{
				vec.emplace_back(i);
				vec15.emplace_back(i);
				vec80.emplace_back(i);
				vec300.emplace_back(i);
				vec1000.emplace_back(i);
			}
		}
	};
}