#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Containers/SmallVec.h"
#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include <algorithm>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSmallVec
{
	TEST_CLASS(TestVector)
	{
	public:
		static constexpr int count = 100000;
		static constexpr size_t sc = count;

		template<class Arg, int... idx>
		void fillInPack(Arg& arg, int val, std::index_sequence<idx...>)
		{
			auto f = { std::get<idx>(arg).emplace_back(val)... };
		}

		// Fill variable number of vectors with a sequence up to count
		template<class... Args>
		void fillVectors(int cnt, Args&& ...args)
		{
			using seq = std::index_sequence_for<Args...>;
			auto arguments = std::forward_as_tuple<Args...>(args...);

			for (int i = 0; i < cnt; ++i)
				fillInPack(arguments, i, seq{});
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

			fillVectors(count, vec, vec15, vec80, vec300, vec1000);

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

			fillVectors(count, vec, vec15, vec80, vec300, vec1000);

			int i = 0;
			for (auto it = vec.begin(); it != vec.end(); ++it, ++i)
				Assert::AreEqual(i, *it, L"vec1");
			i = 0;
			for (auto it = vec15.begin(); it != vec15.end(); ++it, ++i)
				Assert::AreEqual(i, *it, L"vec15");
			i = 0;
			for (auto it = vec80.begin(); it != vec80.end(); ++it, ++i)
				Assert::AreEqual(i, *it, L"vec80");
			i = 0;
			for (auto it = vec300.begin(); it != vec300.end(); ++it, ++i)
				Assert::AreEqual(i, *it, L"vec300");
			i = 0;
			for (auto it = vec1000.begin(); it != vec1000.end(); ++it, ++i)
				Assert::AreEqual(i, *it, L"vec1000");
		}

		TEST_METHOD(SmallVector__OperatorEq)
		{
			SmallVec<int, 1> vec1;
			SmallVec<size_t, 7> vec7;
			SmallVec<int, 15> vec15;
			SmallVec<size_t, 80> vec80;
			SmallVec<int, 301> vec301;
			SmallVec<size_t, 1300> vec1300;

			fillVectors(count, vec1, vec15, vec301);

			vec7 = vec1;
			vec80 = vec15;
			vec1300 = vec301;

			std::cout << vec1.back() << " " << vec7.back();

			Assert::AreEqual(size_t(vec1.back()), vec7.back());
			Assert::AreEqual(size_t(vec15.back()), vec80.back());
			Assert::AreEqual(size_t(vec301.back()), vec1300.back());

			Assert::AreEqual(vec1.size(), vec7.size());
			Assert::AreEqual(vec15.size(), vec80.size());
			Assert::AreEqual(vec301.size(), vec1300.size());

			Assert::AreEqual(vec1.capacity(), vec7.capacity());
			Assert::AreEqual(vec15.capacity(), vec80.capacity());
			Assert::AreEqual(vec301.capacity(), vec1300.capacity());

			vec1[0] += 1;

			Assert::AreNotEqual(size_t(vec1[0]), vec7[0]);

			for (int i = 1; i < count; ++i)
			{
				Assert::AreEqual(size_t(vec1[i]), vec7[i]);
				Assert::AreEqual(size_t(vec15[i]), vec80[i]);
				Assert::AreEqual(size_t(vec301[i]), vec1300[i]);
			}
		}

		TEST_METHOD(SmallVector__SmallSizeCopy)
		{
			// Test small copies that are within the bounds of
			// aligned storage in the small vectors
			static constexpr int count = 65;

			SmallVec<int, 1> vec1;
			SmallVec<int, 70> vec70;

			SmallVec<int, 75> vec75;
			SmallVec<int, 150> vec150;

			fillVectors(count, vec70, vec150);

			vec1 = vec70;
			vec75 = vec150;

			Assert::AreEqual(vec70.back(), vec1.back());
			Assert::AreEqual(vec75.back(), vec150.back());

			Assert::AreEqual(vec70.size(), vec1.size());
			Assert::AreEqual(vec75.size(), vec150.size());

			// This is not guarenteed by the container if we wish to preserve the ability
			// to not allocate more storage if our aligned storage is sufficient
			//
			//Assert::AreEqual(vec70.capacity(), vec1.capacity()); 
			//Assert::AreEqual(vec75.capacity(), vec150.capacity()); 

			for (int i = 0; i < count; ++i)
			{
				Assert::AreEqual(vec70[i], vec1[i]);
				Assert::AreEqual(vec75[i], vec150[i]);
			}
		}

		TEST_METHOD(SmallVector__Emplace)
		{
			SmallVec<int, 1> vec1;
			SmallVec<int, 12> vec12;

			vec1.emplace(0, 2);
			vec1.emplace(1, 3);
			vec1.emplace(0, 1);
			vec1.emplace(0, 0);
			vec1.emplace(4, 4);

			Assert::AreEqual(0, vec1[0]);
			Assert::AreEqual(1, vec1[1]);
			Assert::AreEqual(2, vec1[2]);
			Assert::AreEqual(3, vec1[3]);
			Assert::AreEqual(4, vec1[4]);

			for (int i = 0; i <= 25; ++i)
				vec12.emplace(i, i);

			for (int i = 0; i <= 25; ++i)
				Assert::AreEqual(i, vec12[i]);
		}

		// Test random block iterator erasing
		TEST_METHOD(SmallVector__EraseItMulti)
		{
			const int size = 8000;
			std::vector<int> test;
			SmallVec<int, 1> vec1;
			SmallVec<int, 7> vec7;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 301> vec301;
			SmallVec<int, 1300> vec1300;

			fillVectors(size, test, vec1, vec7, vec15, vec80, vec301, vec1300);

			for (int i = 0; i < size; ++i)
			{
				auto r = rand() % test.size();
				auto rr = r + rand() % 135;

				if (rr < test.size())
				{
					auto it = test.begin() + r;
					auto itL = test.begin() + rr;
					auto v1 = vec1.begin() + r;
					auto v1L = vec1.begin() + rr;
					auto v7 = vec7.begin() + r;
					auto v7L = vec7.begin() + rr;
					auto v15 = vec15.begin() + r;
					auto v15L = vec15.begin() + rr;
					auto v80 = vec80.begin() + r;
					auto v80L = vec80.begin() + rr;
					auto v301 = vec301.begin() + r;
					auto v301L = vec301.begin() + rr;
					auto v1300 = vec1300.begin() + r;
					auto v1300L = vec1300.begin() + rr;

					it = test.erase(it, itL);
					v1 = vec1.erase(v1, v1L);
					v7 = vec7.erase(v7, v7L);
					v15 = vec15.erase(v15, v15L);
					v80 = vec80.erase(v80, v80L);
					v301 = vec301.erase(v301, v301L);
					v1300 = vec1300.erase(v1300, v1300L);

					Assert::AreEqual(*it, *v1);
					Assert::AreEqual(*it, *v7);
					Assert::AreEqual(*it, *v15);
					Assert::AreEqual(*it, *v80);
					Assert::AreEqual(*it, *v301);
					Assert::AreEqual(*it, *v1300);
				}

				Assert::AreEqual(test.size(), vec1.size());
				Assert::AreEqual(test.size(), vec7.size());
				Assert::AreEqual(test.size(), vec15.size());
				Assert::AreEqual(test.size(), vec80.size());
				Assert::AreEqual(test.size(), vec301.size());
				Assert::AreEqual(test.size(), vec1300.size());
			}
		}

		TEST_METHOD(SmallVector__ItAndErase)
		{
			const int size = 20000;
			std::vector<int> test;
			SmallVec<int, 1> vec1;
			SmallVec<int, 7> vec7;
			SmallVec<int, 15> vec15;
			SmallVec<int, 80> vec80;
			SmallVec<int, 301> vec301;
			SmallVec<int, 1300> vec1300;

			fillVectors(size, test, vec1, vec7, vec15, vec80, vec301, vec1300);

			for (int i = 0; i < size; ++i)
			{
				auto r = rand() % test.size();
				auto it = std::lower_bound(test.begin(), test.end(), r);

				size_t dif = it - test.begin();

				// Test iterator use as well as random numbers being in the same places in all vectors
				auto v1i = std::lower_bound(vec1.begin(), vec1.end(), r);
				auto v7i = std::lower_bound(vec7.begin(), vec7.end(), r);
				auto v15i = std::lower_bound(vec15.begin(), vec15.end(), r);
				auto v80i = std::lower_bound(vec80.begin(), vec80.end(), r);
				auto v301i = std::lower_bound(vec301.begin(), vec301.end(), r);
				auto v1300i = std::lower_bound(vec1300.begin(), vec1300.end(), r);

				size_t v1 = v1i - vec1.begin();
				size_t v7 = v7i - vec7.begin();
				size_t v15 = v15i - vec15.begin();
				size_t v80 = v80i - vec80.begin();
				size_t v301 = v301i - vec301.begin();
				size_t v1300 = v1300i - vec1300.begin();

				Assert::AreEqual(dif, v1, L"vec1");
				Assert::AreEqual(dif, v7, L"vec7");
				Assert::AreEqual(dif, v15, L"vec15");
				Assert::AreEqual(dif, v80, L"vec80");
				Assert::AreEqual(dif, v301, L"vec301");
				Assert::AreEqual(dif, v1300, L"vec1300");

				it = test.erase(it);
				v1i = vec1.erase(v1i);
				v7i = vec7.erase(v7i);
				v15i = vec15.erase(v15i);
				v80i = vec80.erase(v80i);
				v301i = vec301.erase(v301i);
				v1300i = vec1300.erase(v1300i);
			}
		}

		TEST_METHOD(SmallVector__FastErase)
		{
			SmallVec<int, 1> vec1;
			SmallVec<int, 6> vec7;

			fillVectors(20, vec1, vec7);

			// 19 is moved into 0 position
			// size is 19
			vec1.fast_erase(vec1.begin());
			vec7.fast_erase(vec7.begin());

			Assert::AreEqual(19, vec1[0]);
			Assert::AreEqual(19, vec7[0]);
			Assert::AreEqual(19ULL, vec1.size());
			Assert::AreEqual(19ULL, vec7.size());
			Assert::AreEqual(18, vec1.back());
			Assert::AreEqual(18, vec7.back());

			vec1.fast_erase(vec1.end() - 1);
			vec7.fast_erase(vec7.end() - 1);

			Assert::AreEqual(18ULL, vec1.size());
			Assert::AreEqual(18ULL, vec7.size());
			Assert::AreEqual(17, vec1.back());
			Assert::AreEqual(17, vec7.back());

			vec1.fast_erase(vec1.begin() + 5);
			vec7.fast_erase(vec7.begin() + 5);

			Assert::AreEqual(17, vec1[5]);
			Assert::AreEqual(17, vec7[5]);
			Assert::AreEqual(16, vec1.back());
			Assert::AreEqual(16, vec7.back());
		}

		
		TEST_METHOD(SmallVector__ShrinkToFit)
		{
			SmallVec<int, 1> vec1;
			SmallVec<int, 6> vec7;

			fillVectors(20, vec1, vec7);

			vec1.erase(vec1.begin(), vec1.begin() + 5);
			vec7.erase(vec7.begin(), vec7.begin() + 5);

			vec1.shrink_to_fit();
			vec7.shrink_to_fit();

			Assert::AreEqual(vec1.capacity(), 15Ull);
			Assert::AreEqual(vec1.size(), vec1.capacity());
			Assert::AreEqual((vec1.begin() + 15).ptr, vec1.end().ptr);

			Assert::AreEqual(vec7.capacity(), 15Ull);
			Assert::AreEqual(vec7.size(), vec7.capacity());
			Assert::AreEqual((vec7.begin() + 15).ptr, vec7.end().ptr);

			vec1.erase(vec1.begin() + 5, vec1.begin() + 9);
			vec7.erase(vec7.begin() + 5, vec7.begin() + 9);

			vec1.shrink_to_fit();
			vec7.shrink_to_fit();

			Assert::AreEqual(vec1.capacity(), 11Ull);
			Assert::AreEqual(vec1.size(), vec1.capacity());
			Assert::AreEqual((vec1.begin() + 11).ptr, vec1.end().ptr);

			Assert::AreEqual(vec7.capacity(), 11Ull);
			Assert::AreEqual(vec7.size(), vec7.capacity());
			Assert::AreEqual((vec7.begin() + 11).ptr, vec7.end().ptr);
		}
		
		TEST_METHOD(SmallVector__ShrinkToFitAligned)
		{
			SmallVec<int, 10> vec;

			fillVectors(20, vec);

			vec.erase(vec.begin() + 9, vec.end());

			vec.shrink_to_fit();

			Assert::AreEqual(9ULL, vec.size());
			Assert::AreEqual(vec.size(), vec.capacity());
			Assert::AreEqual(vec.back(), *(vec.end() - 1));
		}
	};
}