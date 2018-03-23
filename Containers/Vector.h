#pragma once
#include <type_traits>

// TODO: Allocator
// TODO: Iterators
// TODO: copy / move
// TODO: emplace_myBack

template<class C> // Faster for int's by about 15%!
class Vector
{
public:
	Vector() = default;
	Vector(int size) : capacity(size)
	{
		myStart = new C[size];
		myBack = myEnd = myStart[0]; 
	}

	C& operator[](const int idx)
	{
		return myStart[idx];
	}

	void resize(const int newSize)
	{
		if (newSize <= capacity)
			return;

		C *n = new C[newSize];

		for (int i = 0; i < _size; ++i) 
		{
			n[i] = myStart[i];
		}

		delete[] myStart;

		myStart  = &n[0];
		myBack   = &n[_size];
		myEnd    = &n[newSize];
		capacity = newSize;
	}

	void emplace_back(C &&c)
	{
		checkForCapacity();

		*myBack = c;
		++myBack;
		++_size;
	}

	void push_back(const C& c)
	{
		emplace_back(std::move(c));
	}

	int size() { return _size; }
	bool empty() { return _size; }

	C* begin(){ return myStart; } // TODO: Give Iterators not ptrs
	C* end() { return myBack; }

	~Vector()
	{
		delete[] myStart;
	}

private:

	inline void checkForCapacity()
	{
		if (myBack != myEnd) 
		{}
		else
			resize((capacity * 2) + 1);
	}

	int capacity = 0;
	int _size = 0;
	C * myStart;
	C * myBack;
	C * myEnd;
};