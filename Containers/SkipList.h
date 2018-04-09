#pragma once
#include <stdint.h>
#include <memory>
#include "OrderedStructuresHelpers.h"

template<class Traits>
struct SkipNode
{

};

template<class Type>
struct SkipListNode
{
	int height;

	SkipListNode* down;
	SkipListNode* right; // TODO: Use an array of two here? Faster or slower?

	Type data;
};

// TODO: Flat array storage, using indexes instead of pointers?

template<class Traits>
class SkipList
{
public:
	using MyBase = SkipList<Traits>;
	using BaseTypes = OrderedTypes<Traits, SkipListNode>;

	using Node			  = typename BaseTypes::Node;
	using NodePtr		  = typename BaseTypes::NodePtr;
	using difference_type = typename BaseTypes::difference_type;
	using value_type      = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer	  = typename BaseTypes::const_pointer;
	using reference		  = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using key_type		= typename BaseTypes::key_type;
	using key_compare   = typename Traits::key_compare;
	using value_compare = typename Traits::value_compare;
	using get_key		= typename Traits::get_key;
	using size_type     = size_t;

	using NodeAl	   = typename BaseTypes::NodeAl;
	using NodeAlTraits = typename BaseTypes::NodeAlTraits;

private:
	NodeAl nodeAl; 

	NodePtr Head;

	get_key getKey;
	key_compare compare;

	enum { DOWN, RIGHT };

	static bool xorshift128plus() const noexcept // TODO: Test distribution of different bits for our use case
	{	// From the wiki xorshift
		static constexpr uint64_t andBit = 1 << 5;
		uint64_t x = s[0];
		const uint64_t y = s[1];
		s[0] = y;
		x ^= x << 23; // a
		s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
		return (s[1] + y) & andBit;
	}

	static int generateHeight() const noexcept
	{
		int n = 0;
		while (xorshift128plus())
			++n;

		return n;
	}

	template<class... Args>
	NodePtr createNode(Args&& ...args) // TODO: Exception handling
	{
		NodePtr n = nodeAl.allocate(1);
		//NodeAlTraits::construct(nodeAl, std::addressof(n->height), height);
		NodeAlTraits::construct(nodeAl, std::addressof(n->down), nullptr);
		NodeAlTraits::construct(nodeAl, std::addressof(n->right), nullptr);
		NodeAlTraits::construct(nodeAl, std::addressof(n->data), std::forward<Args>(args)...);

		// TODO: Exception handling
		return n; 
	}

	template<class... Args>
	void tryEmplace(Args&& ...args)
	{
		NodePtr p = createNode(std::forward<Args>(args)...);

		int curHeight = 0;
		const int desHeight = generateHeight();

		NodePtr cur = Head;

		for (;;)
		{
			int dir = compare(getKey(cur->data), getKey(p->data));

			if (dir == RIGHT)
				cur = cur->right;
			else
				cur = cur->down;
		}
	}
};