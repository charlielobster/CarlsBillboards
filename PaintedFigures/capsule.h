#pragma once

#include "doublePoint.h"
#include "doubleLine.h"
#include "doubleTriangle.h"
#include "doubleRect.h"

class capsule
{
public:
	doubleLine line;
	doubleRect rect;
	double radius;
	double min;
	double max;
	capsule() {}

	inline void init()
	{
		double dy = line.pq.y;
		double dx = line.pq.x;
		double d = line.pq.d;
		double s = radius * dx / d;
		double t = radius * dy / d;

		rect.a.x = line.p.x - s;
		rect.a.y = line.p.y - t;
		rect.b.x = line.p.x + s;
		rect.b.y = line.p.y + t;
		rect.c.x = line.q.x - s;
		rect.c.y = line.q.y - t;
		rect.d.x = line.q.x + s;
		rect.d.y = line.q.y + t;

		rect.a.set(); rect.b.set(); rect.c.set(); rect.d.set();
		rect.set();

		min = min(line.min, rect.min);
		max = max(line.max, rect.max);
	}

	capsule(const capsule &c) : line(c.line), radius(c.radius) { init(); }
	capsule(const doubleLine &l, double r) : line(l), radius(r) { init(); }
	capsule(const doublePoint &p, const doublePoint &q, double radius) : line(p, q), radius(radius) { init(); }

	inline capsule &operator=(const capsule &c)
	{
		if (this == &c) return *this;
		line = c.line;
		radius = c.radius;
		init();
		return *this;
	}

	inline friend const capsule &operator-(const capsule &c, double d)
	{
		return capsule(c.line - d, c.radius);
	}

	inline friend const capsule &operator/(const capsule &c, double d)
	{
		return capsule(c.line / d, c.radius / d);
	}
};