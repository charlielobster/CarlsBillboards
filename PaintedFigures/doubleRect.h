#pragma once

#include <assert.h>
#include "doublePoint.h"

#define MIN_RECT doubleRect(MIN_POINT, MIN_POINT, MIN_POINT, MIN_POINT)

class doubleRect
{
public:
	doublePoint a;
	doublePoint b;
	doublePoint c;
	doublePoint d;
	doublePoint minPoint;
	doublePoint maxPoint;
	double min;
	double max;
	double area;
	doubleRect() {}

	void set()
	{
		minPoint = doublePoint(
			min(a.x, min(b.x, min(c.x, d.x))),
			min(a.y, min(b.y, min(c.y, d.y)))
		);
		maxPoint = doublePoint(
			max(a.x, max(b.x, max(c.x, d.x))),
			max(a.y, max(b.y, max(c.y, d.y)))
		);
		min = min(minPoint.x, minPoint.y);
		max = max(maxPoint.x, maxPoint.y);
		doublePoint ab = doublePoint(a - b);
		doublePoint bc = doublePoint(b - c);
		area = ab.d * bc.d;
	}

	doubleRect(const doubleRect &r) : a(r.a), b(r.b), c(r.c), d(r.d) 
	{ 
		set(); 
	}

	doubleRect(const doublePoint &pa, const doublePoint &pb,
		const doublePoint &pc, const doublePoint &pd) : a(pa), b(pb), c(pc), d(pd)
	{
		set();
	}

	doubleRect &operator=(const doubleRect &r)
	{
		if (this == &r) return *this;
		a = r.a; b = r.b; c = r.c; d = r.d;
		set();
		return *this;
	}

	const doublePoint &operator[](unsigned short i) const
	{
		assert(i < 4);
		switch (i) {
			case 0: return a;
			case 1: return b;
			case 2: return c;
			default: return d;
		}
	}

	const bool contains(const doublePoint &p) const
	{
		return (minPoint.x <= p.x && p.x <= maxPoint.x && 
			minPoint.y <= p.y && p.y <= maxPoint.y);
	}

	friend const doubleRect operator*(const doubleRect &r, double t)
	{
		return doubleRect(r.a * t, r.b * t, r.c * t, r.d * t);
	}

	friend const doubleRect intersection(const doubleRect &l, const doubleRect &r)
	{
		double max_minx = max(l.minPoint.x, r.minPoint.x);
		double max_miny = max(l.minPoint.y, r.minPoint.y);
		double min_maxx = min(l.maxPoint.x, r.maxPoint.x);
		double min_maxy = min(l.maxPoint.y, r.maxPoint.y);

		if (max_minx < min_maxx && max_miny < min_maxy) {
			return doubleRect(
				doublePoint(max_minx, max_miny),
				doublePoint(max_minx, min_maxy),
				doublePoint(min_maxx, min_maxy),
				doublePoint(min_maxx, max_miny)
			);
		} else {
			return MIN_RECT;
		}
	}
};
