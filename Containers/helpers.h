#pragma once
#include <iostream>
#include <chrono>
#include <type_traits>

#include <xutility>

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>;

template<class Ptr>
using PtrType = typename std::remove_reference<decltype(*std::declval<Ptr>())>::type;

// Move elements between (first, end]
// the first element being placed at dest,
// the last at: dest + (end - first)
template<class PtrIn, class PtrOut>
inline PtrOut uncheckedMove(PtrIn first, PtrIn end, PtrOut dest) //noexcept std::move_if_noexcept()?
{
	using Type1 = PtrType<PtrIn >;
	using Type2 = PtrType<PtrOut>;

	// memmove optimization taken from MCVS std::
	if constexpr(std::is_trivially_assignable_v<PtrOut&, PtrIn> 
		&& (sizeof(Type1) == sizeof(Type2) 
		&& std::is_integral_v<Type1>
		&& std::is_integral_v<Type2>
		&& std::is_same_v<Type1, bool> == std::is_same_v<Type2, bool>))
	{	
		const char * const firstCh = const_cast<const char *>(reinterpret_cast<const volatile char *>(first));
		const char * const lastCh  = const_cast<const char *>(reinterpret_cast<const volatile char *>(end  ));
		      char * const destCh  = const_cast<char       *>(reinterpret_cast<      volatile char *>(dest ));

		const size_t numBytes = lastCh - firstCh;
		std::memmove(destCh, firstCh, numBytes);

		return reinterpret_cast<PtrOut>(destCh + numBytes); 
	}
	else
	{
		for (first; first != end; ++first, ++dest)
			*dest = std::move(*first);
	}

	return dest;
}

// Call destructor for elements between pointers
// unless there's no need to
// [first, end)
template<class Alloc, class Ptr>
void destroyRange(Alloc al, Ptr first, Ptr end)
{
	if constexpr(std::is_trivially_destructible<PtrType<Ptr>>::value)
		;
	else
	{
		for (first; first != end; ++first)
			al.destroy(first);
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