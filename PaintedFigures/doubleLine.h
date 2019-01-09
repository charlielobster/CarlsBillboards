#pragma once

#include "doublePoint.h"

class doubleLine
{
public:
	doublePoint p;
	doublePoint q;
	doublePoint pq;
	doubleLine() {}
	doubleLine(double d) : p(d), q(d), pq(0) {}
	doubleLine(const doubleLine &l) : p(l.p), q(l.q), pq(l.pq) {}
	doubleLine(const doublePoint &p, const doublePoint &q) : p(p), q(q) { pq = q - p; }

	doubleLine(double px, double py, double qx, double qy) : p(px, py), q(qx, qy), pq(q - p)
	{
		double dy = pq.y;
		double dx = pq.x;
		double abs = pq.distance();
	}

	inline doubleLine &operator=(const doubleLine &l)
	{
		if (this == &l) return *this;
		p = l.p; q = l.q; pq = l.pq;
		return *this;
	}

	inline doubleLine operator*(double d)
	{
		return doubleLine(p * d, q * d);
	}

	inline doublePoint &operator[](int i) { return (i == 0 ? p : q); }

	inline double m() const
	{
		return (pq.y / pq.x);
	}

	inline double b() const
	{
		return p.y - m() * p.x;
	}

	inline double minValue() const
	{
		return min(p.minValue(), q.minValue());
	}

	inline double maxValue() const
	{
		return max(p.maxValue(), q.maxValue());
	}

	inline const doubleLine normalized(double min, double d)
	{
		return doubleLine(p.normalized(min, d), q.normalized(min, d));
	}

	inline friend const doublePoint &intersection(const doubleLine &l1, const doubleLine &l2)
	{
		double x = (l2.b() - l1.b()) / (l1.m() - l2.m());
		double y = l1.m() * x + l1.b();
		return doublePoint(x, y);
	}
};