#pragma once

#define M_PI 3.1415926535897932384626433832795 

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
	double area;
	capsule() {}

	void set()
	{
		double dy = line.dy;
		double dx = line.dx;
		double d = line.d;
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
		area = 2.0 * radius * line.d + M_PI * radius * radius;
	}

	capsule(const capsule &c) : line(c.line), radius(c.radius) { set(); }
	capsule(const doubleLine &l, double r) : line(l), radius(r) { set(); }
	capsule(const doublePoint &p, const doublePoint &q, double radius) : line(p, q), radius(radius) { set(); }

	capsule &operator=(const capsule &c)
	{
		if (this == &c) return *this;
		line = c.line;
		radius = c.radius;
		set();
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