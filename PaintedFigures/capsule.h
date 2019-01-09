#pragma once

#include "doubleLine.h"
#include "doubleTriangle.h"
#include "doubleRect.h"

class capsule
{
public:
	doubleLine line;
	double radius;
	doubleRect rect;
	capsule() {}
	capsule(const capsule &c) : line(c.line), radius(c.radius) { init(); }
	capsule(const doubleLine &l, double r) : line(l), radius(r) { init(); }
	capsule(const doublePoint &p, const doublePoint &q, double radius) : line(p, q), radius(radius) { init(); }

	inline capsule &operator=(const capsule &c)
	{
		if (this == &c) return *this;
		line = c.line;
		radius = c.radius;
		rect = c.rect;
		return *this;
	}

	inline double minValue() const
	{
		return min(line.minValue(), rect.minValue());
	}

	inline double maxValue() const
	{
		return min(line.maxValue(), rect.maxValue());
	}

	inline capsule normalized(double min, double d)
	{
		capsule c;
		c.line = line.normalized(min, d);
		c.rect = rect.normalized(min, d);
		c.radius = radius / d;
		return c;
	}

private:
	inline void init()
	{
		double dy = line.pq.y;
		double dx = line.pq.x;
		double abs = line.pq.distance();

		rect.vertices[0].x = line.p.x - radius * dx / abs;
		rect.vertices[0].y = line.p.y - radius * dy / abs;
		rect.vertices[1].x = line.p.x + radius * dx / abs;
		rect.vertices[1].y = line.p.y + radius * dy / abs;
		rect.vertices[2].x = line.q.x - radius * dx / abs;
		rect.vertices[2].y = line.q.y - radius * dy / abs;
		rect.vertices[3].x = line.q.x + radius * dx / abs;
		rect.vertices[3].y = line.q.y + radius * dy / abs;
	}
};