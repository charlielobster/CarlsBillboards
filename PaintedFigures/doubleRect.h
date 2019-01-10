#pragma once

#include <assert.h>
#include "doublePoint.h"

class doubleRect
{
public:
	doublePoint a;
	doublePoint b;
	doublePoint c;
	doublePoint d;
	double min;
	double max;
	doubleRect() {}

	inline void set()
	{
		min = min(min(min(a.min, b.min), c.min), d.min);
		max = max(max(max(a.max, b.max), c.max), d.max);
	}

	doubleRect(const doubleRect &r) : a(r.a), b(r.b), c(r.c), d(r.d) { set(); }

	doubleRect(const doublePoint &pa, const doublePoint &pb, 
		const doublePoint &pc, const doublePoint &pd) : a(pa), b(pb), c(pc), d(pd) 
	{
		set();
	}

	inline doubleRect &operator=(const doubleRect &r)
	{
		if (this == &r) return *this;
		a = r.a; b = r.b; c = r.c; d = r.d;
		set();
		return *this;
	}

	inline friend const doubleRect &operator*(const doubleRect &r, double t)
	{
		return doubleRect(r.a * t, r.b * t, r.c * t, r.d * t);
	}

	inline const doublePoint &operator[](unsigned short i) const 
	{
		assert(i < 4);
		switch (i) {
			case 0: return a;
			case 1: return b;
			case 2: return c;
			default: return d;
		}
	}
};
