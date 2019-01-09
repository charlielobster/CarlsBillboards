#pragma once

#include "doublePoint.h"

class doubleRect
{
public:
	doublePoint vertices[4];
	doubleRect() {}

	doubleRect(const doubleRect &r)
	{
		vertices[0] = r.vertices[0];
		vertices[1] = r.vertices[1];
		vertices[2] = r.vertices[2];
		vertices[3] = r.vertices[3];
	}

	doubleRect(const doublePoint &a, const doublePoint &b, const doublePoint &c, const doublePoint &d) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		vertices[3] = d;
	}

	inline doubleRect &operator=(const doubleRect &r)
	{
		if (this == &r) return *this;
		vertices[0] = r.vertices[0];
		vertices[1] = r.vertices[1];
		vertices[2] = r.vertices[2];
		vertices[3] = r.vertices[3];
		return *this;
	}

	inline doubleRect operator*(double d)
	{
		return doubleRect(vertices[0] * d, vertices[1] * d, vertices[2] * d, vertices[3] * d);
	}

	inline double minValue() const
	{
		return min(min(min(vertices[0].minValue(), 
			vertices[1].minValue()), 
			vertices[2].minValue()), 
			vertices[3].minValue());
	}

	inline double maxValue() const
	{
		return max(max(max(vertices[0].maxValue(), 
			vertices[1].maxValue()), 
			vertices[2].maxValue()), 
			vertices[3].maxValue());
	}

	inline const doubleRect &normalized(double min, double d)
	{
		return doubleRect(vertices[0].normalized(min, d), 
			vertices[1].normalized(min, d), 
			vertices[2].normalized(min, d), 
			vertices[3].normalized(min, d));
	}

	inline doublePoint &operator[](int i) { return vertices[i]; }
};
