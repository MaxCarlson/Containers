#pragma once
#include <deque>

std::deque<int> ff;

template<class Type>
class Deque
{
public:
	Deque()
	{

	}


private:
	Type **arrLocs;
	Type * MyFirst;
	Type * MyLast;
};