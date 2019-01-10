#pragma once

#include "doublePoint.h"
#include "doubleLine.h"

class doubleTriangle
{
public:
	doublePoint a;
	doublePoint b;
	doublePoint c;
	doubleTriangle() {}

	doubleTriangle(const doubleTriangle &t) : a(t.a), b(t.b), c(t.c) {}

	doubleTriangle(const doublePoint &ta, const doublePoint &tb, const doublePoint &tc) : a(ta), b(tb), c(tc) {}

	inline doubleTriangle &operator=(const doubleTriangle &l)
	{
		if (this == &l) return *this;
		a = l.a; b = l.b; c = l.c;
		return *this;
	}

	inline const doublePoint &operator[](unsigned short i) const 
	{
		assert(i < 3); 
		switch (i) { 
			case 0: return a; 
			case 1: return b; 
			default: return c;
		}
	}
};


