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
	double dy;
	double dx;
	double d;
	double min;
	double max;
	doubleLine() {}

	void set()
	{
		pq = q - p;
		dy = pq.y;
		dx = pq.x;
		d = pq.d;
		min = min(p.min, q.min);
		max = max(p.max, q.max);
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

	friend const doublePoint intersection(const doubleLine &l1, const doubleLine &l2)
	{
		assert((l1.m - l2.m) != 0.0);
		double x = (l2.b - l1.b) / (l1.m - l2.m);
		double y = l1.m * x + l1.b; 
		return doublePoint(x, y);
	}
};