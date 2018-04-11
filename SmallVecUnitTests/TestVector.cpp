#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Containers/SmallVec.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SmallVecUnitTests
{		
	TEST_CLASS(TestVector)
	{
	public:
		
		TEST_METHOD(TestInsert)
		{
			constexpr int count = 100000;
			constexpr size_t sc = count;

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

	};
}