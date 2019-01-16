#pragma once

#include "doublePoint.h"
#include "doubleRect.h"

class doubleLine
{
public:
	doublePoint p;
	doublePoint q;
	doublePoint pq;
	double m;
	double b;
	double dy;
	double dx;
	double d;
	doublePoint minPoint;
	doublePoint maxPoint;
	doubleRect bounds;

	double min;
	double max;
	doubleLine() {}

	void set()
	{
		pq = q - p;
		dy = pq.y;
		dx = pq.x;
		d = pq.d;
		minPoint = doublePoint(min(p.x, q.x), min(p.y, q.y));
		maxPoint = doublePoint(max(p.x, q.x), max(p.y, q.y));
		bounds = doubleRect(
			minPoint,
			doublePoint(minPoint.x, maxPoint.y),
			maxPoint,
			doublePoint(maxPoint.x, minPoint.y)
		);
		min = min(minPoint.x, minPoint.y);
		max = max(maxPoint.x, maxPoint.y);
		m = pq.m;
		b = p.y - m * p.x;
	}

	doubleLine(double d) : p(d), q(d) { set(); }
	doubleLine(const doubleLine &l) : p(l.p), q(l.q) { set(); }
	doubleLine(const doublePoint &p, const doublePoint &q) : p(p), q(q) { set(); }
	doubleLine(double px, double py, double qx, double qy) : p(px, py), q(qx, qy) { set(); }

	doubleLine &operator=(const doubleLine &l)
	{
		if (this == &l) return *this;
		p = l.p; q = l.q; set();
		return *this;
	}

	const doublePoint &operator[](int i) const { return (i == 0 ? p : q); }

	friend bool operator==(const doubleLine &l, const doubleLine &r)
	{
		return (l.p == r.p && l.q == r.q);
	}

	friend bool operator!=(const doubleLine &l, const doubleLine &r)
	{
		return (l.p.x != r.p.x || l.p.y != r.p.y || l.q.x != r.q.x || l.q.y != r.q.y);
	}

	friend const doubleLine operator-(const doubleLine &l, double d)
	{
		return doubleLine(l.p - d, l.q - d);
	}

	friend const doubleLine operator*(const doubleLine &l, double d)
	{
		return doubleLine(l.p * d, l.q * d);
	}

	friend const doubleLine operator/(const doubleLine &l, double d)
	{
		return doubleLine(l.p / d, l.q / d);
	}

	friend bool colinear(const doubleLine &l, const doubleLine &r)
	{
		return ((l == r) || (r.q == l.p && r.p == l.q));
	}

	friend const doublePoint intersection(const doubleLine &l, const doubleLine &r)
	{
		// get the intersection rect of the lines' bounds
		doubleRect i = intersection(l.bounds, r.bounds);
		if (i.area != 0) { 	// the lines completely miss if the intersection rect area is 0, 
			assert((l.m - r.m) != 0.0);
			double x = (r.b - l.b) / (l.m - r.m); // use y = mx + b to find the intersection point
			double y = l.m * x + l.b;
			doublePoint p(x, y);
			if (i.contains(p)) { // the point must lie inside the intersection rect
				return doublePoint(x, y);
			}
		}
		return MIN_POINT;
	}
};