#pragma once
#include <iostream>
#include <chrono>
#include <xmmintrin.h> 
#include <mmintrin.h> 

template<class T>
inline void prefetch(T * n)
{
	_mm_prefetch((const char *)n, _MM_HINT_T0);
}

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>;


template<class C>
inline void timer(bool startNow)
{
	using namespace std::chrono;

	static std::chrono::time_point<std::chrono::steady_clock> start;

	if (startNow)
	{
		start = high_resolution_clock::now();
		return;
	}

	std::cout << duration_cast<duration<double>>(high_resolution_clock::now() - start).count() << "\n";
}