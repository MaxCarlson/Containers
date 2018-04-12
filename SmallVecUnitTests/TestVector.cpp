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

		const wchar_t* equalError(std::string name, int expected, int real)
		{
			std::stringstream ss;
			ss << name << " expected: " << expected << " real: " << real;
			auto&& str = ss.str();
			std::wstring w(str.size(), L' ');
			std::copy(str.begin(), str.end(), w.begin());

			return w.c_str();
		}

		TEST_METHOD(SmallVector__TestInsert)
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

				Assert::AreEqual(i, vec[i], equalError("vec1", i, vec[i]));
				Assert::AreEqual(i, vec15[i], equalError("vec15", i, vec15[i]));
				Assert::AreEqual(i, vec80[i], equalError("vec80", i, vec80[i]));
				Assert::AreEqual(i, vec300[i], equalError("vec300", i, vec300[i]));
				Assert::AreEqual(i, vec1000[i], equalError("vec1000", i, vec1000[i]));
			}
			Assert::AreEqual(sc, vec.size(), equalError("vec1", sc, vec.size()));
			Assert::AreEqual(sc, vec15.size(), equalError("vec1", sc, vec15.size()));
			Assert::AreEqual(sc, vec80.size(), equalError("vec1", sc, vec80.size()));
			Assert::AreEqual(sc, vec300.size(), equalError("vec1", sc, vec300.size()));
			Assert::AreEqual(sc, vec1000.size(), equalError("vec1", sc, vec1000.size()));
		}

		TEST_METHOD(SmallVector__TestPopBack)
		{
			SmallVec<int, 1> vec;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 300> vec300;
			SmallVec<int, 1000> vec1000;

			fillVectors(vec, vec15, vec80, vec300, vec1000);

			for (int i = count - 1; i > 0; --i)
			{
				auto t = vec.back();
				vec.pop_back();
				Assert::AreEqual(vec.back(), t - 1);

				t = vec15.back();
				vec15.pop_back();
				Assert::AreEqual(vec15.back(), t - 1);

				t = vec80.back();
				vec80.pop_back();
				Assert::AreEqual(vec80.back(), t - 1);

				t = vec300.back();
				vec300.pop_back();
				Assert::AreEqual(vec300.back(), t - 1);

				t = vec1000.back();
				vec1000.pop_back();
				Assert::AreEqual(vec1000.back(), t - 1);
			}

		}

		TEST_METHOD(SmallVector__Iterating)
		{
			SmallVec<int, 1> vec;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 300> vec300;
			SmallVec<int, 1000> vec1000;

			fillVectors(vec, vec15, vec80, vec300, vec1000);

			int i = 0;
			for (auto it = vec.begin(); it != vec.end(); ++it, ++i)
				Assert::AreEqual(i, *it, equalError("vec1", i, *it));
			i = 0;
			for (auto it = vec15.begin(); it != vec15.end(); ++it, ++i)
				Assert::AreEqual(i, *it, equalError("vec15", i, *it));
			i = 0;
			for (auto it = vec80.begin(); it != vec80.end(); ++it, ++i)
				Assert::AreEqual(i, *it, equalError("vec80", i, *it));
			i = 0;
			for (auto it = vec300.begin(); it != vec300.end(); ++it, ++i)
				Assert::AreEqual(i, *it, equalError("vec300", i, *it));
			i = 0;
			for (auto it = vec1000.begin(); it != vec1000.end(); ++it, ++i)
				Assert::AreEqual(i, *it, equalError("vec1000", i, *it));
		}
	};
}