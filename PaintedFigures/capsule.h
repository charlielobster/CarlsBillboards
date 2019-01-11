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

	void init()
	{
		double dy = line.pq.y;
		double dx = line.pq.x;
		double d = line.pq.d;
		double s = radius * dx / d;
		double t = radius * dy / d;

		rect = doubleRect(
			doublePoint(line.p.x - t, line.p.y + s),
			doublePoint(line.p.x + t, line.p.y - s),
			doublePoint(line.q.x - t, line.q.y + s),
			doublePoint(line.q.x + t, line.q.y - s)
		);

		min = min(line.min, rect.min);
		max = max(line.max, rect.max);
	}

	capsule(const capsule &c) : line(c.line), radius(c.radius) { init(); }
	capsule(const doubleLine &l, double r) : line(l), radius(r) { init(); }
	capsule(const doublePoint &p, const doublePoint &q, double radius) : line(p, q), radius(radius) { init(); }

	capsule &operator=(const capsule &c)
	{
		if (this == &c) return *this;
		line = c.line;
		radius = c.radius;
		init();
		return *this;
	}

	friend capsule operator-(const capsule &c, double d)
	{
		return capsule(c.line - d, c.radius);
	}

	friend capsule operator/(const capsule &c, double d)
	{
		return capsule(c.line / d, c.radius / d);
	}
};