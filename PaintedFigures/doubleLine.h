#pragma once

#include "doublePoint.h"

class doubleLine
{
public:
	doublePoint p;
	doublePoint q;
	doublePoint pq;
	double m;
	double b;
	double min;
	double max;
	doubleLine() {}

	inline void set()
	{
		pq = q - p;
		min = min(p.min, q.min);
		max = max(p.max, q.max);
		m = pq.m;
		b = p.y - m * p.x;
	}

	doubleLine(double d) : p(d), q(d) { set(); }
	doubleLine(const doubleLine &l) : p(l.p), q(l.q) { set(); }
	doubleLine(const doublePoint &p, const doublePoint &q) : p(p), q(q) { set(); }
	doubleLine(double px, double py, double qx, double qy) : p(px, py), q(qx, qy) { set(); }

	inline doubleLine &operator=(const doubleLine &l)
	{
		if (this == &l) return *this;
		p = l.p; q = l.q; set();
		return *this;
	}

	inline const doublePoint &operator[](int i) const { return (i == 0 ? p : q); }

	inline friend const doubleLine operator-(const doubleLine &l, double d)
	{
		return doubleLine(l.p - d, l.q - d);
	}

	inline friend const doubleLine operator*(const doubleLine &l, double d)
	{
		return doubleLine(l.p * d, l.q * d);
	}

	inline friend const doubleLine operator/(const doubleLine &l, double d)
	{
		return doubleLine(l.p / d, l.q / d);
	}

	inline friend const doublePoint intersection(const doubleLine &l1, const doubleLine &l2)
	{
		assert((l1.m - l2.m) != 0.0);
		double x = (l2.b - l1.b) / (l1.m - l2.m);
		double y = l1.m * x + l1.b; 
		return doublePoint(x, y);
	}
};