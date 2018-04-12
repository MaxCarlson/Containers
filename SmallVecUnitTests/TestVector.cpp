#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Containers/SmallVec.h"
#include <utility>
#include <array>
#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SmallVecUnitTests
{		
	TEST_CLASS(TestVector)
	{
	public:
		static constexpr int count = 100000;
		static constexpr size_t sc = count;

		template<class Arg, int... idx>
		void getFromPack(Arg& arg, int val, std::index_sequence<idx...>)
		{
			constexpr std::array<int, sizeof...(idx)> arr = { idx... };

			auto f = { std::get<idx>(arg).emplace_back(val)... };
		}

		// Fill variable number of vectors with a sequence up to count
		template<class... Args>
		void fillVectors(Args&& ...args)
		{
			constexpr int ccc = sizeof...(Args);
			using seq = std::index_sequence_for<Args...>;
			auto arguments = std::forward_as_tuple<Args...>(args...);

			for (int i = 0; i < count; ++i)
				getFromPack(arguments, i, seq{});
		}

		std::wstring equalError(std::string name, int expected, int real)
		{
			std::stringstream ss;
			ss << name << " expected: " << expected << " real: " << real;
			auto&& str = ss.str();
			std::wstring w(str.size(), L' ');
			std::copy(str.begin(), str.end(), w.begin());

			return w;
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

				Assert::AreEqual(i, vec[i], equalError("vec1", i, vec[i]).c_str());
				Assert::AreEqual(i, vec15[i], equalError("vec15", i, vec15[i]).c_str());
				Assert::AreEqual(i, vec80[i], equalError("vec80", i, vec80[i]).c_str());
				Assert::AreEqual(i, vec300[i], equalError("vec300", i, vec300[i]).c_str());
				Assert::AreEqual(i, vec1000[i], equalError("vec1000", i, vec1000[i]).c_str());
			}
			Assert::AreEqual(sc, vec.size(), equalError("vec1", sc, vec.size()).c_str());
			Assert::AreEqual(sc, vec15.size(), equalError("vec1", sc, vec15.size()).c_str());
			Assert::AreEqual(sc, vec80.size(), equalError("vec1", sc, vec80.size()).c_str());
			Assert::AreEqual(sc, vec300.size(), equalError("vec1", sc, vec300.size()).c_str());
			Assert::AreEqual(sc, vec1000.size(), equalError("vec1", sc, vec1000.size()).c_str());
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