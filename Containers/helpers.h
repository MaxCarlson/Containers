#pragma once
#include <iostream>
#include <chrono>
#include <type_traits>

#include <xutility>

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>;

template<class Ptr>
using PtrType = typename std::remove_reference<decltype(*std::declval<Ptr>())>::type;

// Move elements first - last to dest,
// with first getting placed on dest
template<class PtrIn, class PtrOut>
inline PtrOut uncheckedMove(PtrIn first, PtrIn last, PtrOut dest)
{
	using Type1 = PtrType<PtrIn >;
	using Type2 = PtrType<PtrOut>;

	if constexpr(std::is_trivially_assignable_v<PtrOut&, PtrIn>
		&& (sizeof(Type1) == sizeof(Type2)
		&& std::is_integral_v<Type1>
		&& std::is_integral_v<Type2>
		&& std::is_same_v<Type1, bool> == std::is_same_v<Type2, bool>))
	{
		const char * const firstCh = const_cast<const char *>(reinterpret_cast<const volatile char *>(first));
		const char * const lastCh  = const_cast<const char *>(reinterpret_cast<const volatile char *>(last ));
		      char * const destCh  = const_cast<char       *>(reinterpret_cast<      volatile char *>(dest ));

		const size_t numBytes = lastCh - firstCh;
		std::memmove(destCh, firstCh, numBytes);

		return reinterpret_cast<PtrOut>(destCh + numBytes);
	}
	else
	{
		for (first; first != last; ++first, ++last)
			*dest = std::move(*first);
	}

	return dest;
}

template<class Ptr>
void destroyRange(Ptr first, Ptr end)
{
	if constexpr(std::is_trivially_destructible<PtrType<Ptr>>)
	{

	}
	else
	{

	}
}

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