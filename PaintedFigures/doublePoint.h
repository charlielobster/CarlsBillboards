#pragma once

#include <math.h>

#define MAX_DOUBLE 1.79769e+308 
#define MIN_DOUBLE (-1.0 * MAX_DOUBLE)

#define MAX_RADIUS 49
#define MAX_DIMENSION 999
#define MAX_N 49

class doublePoint 
{
public:
	double x;
	double y;
	doublePoint() : x(0.0), y(0.0) {}
	doublePoint(double d) : x(d), y(d) {}
	doublePoint(const doublePoint &p) : x(p.x), y(p.y) {}
	doublePoint(double x, double y) : x(x), y(y) {}

	inline doublePoint &operator=(const doublePoint &p) 
	{ 
		if (this == &p) return *this; 
		x = p.x; y = p.y;
		return *this;
	}

	inline friend const doublePoint &operator-(const doublePoint &q, const doublePoint &p) 
	{ 
		return doublePoint(q.x - p.x, q.y - p.y); 
	}

	inline friend const doublePoint &operator+(const doublePoint &p, const doublePoint &q) 
	{ 
		return doublePoint(p.x + q.x, p.y + q.y); 
	}

	inline double &operator[](int i) { return (i == 0 ? x : y); }

	inline doublePoint operator*(double d)
	{
		return doublePoint(x * d, y * d);
	}

	inline friend double cross(const doublePoint &p, const doublePoint &q) 
	{ 
		return (q.y * p.x - q.x * p.y); 
	}

	inline friend double dot(const doublePoint &p, const doublePoint &q) 
	{ 
		return (q.x * p.x + q.y * p.y); 
	}

	inline double distance() const
	{
		return sqrt(dot(*this, *this));
	}

	inline double minValue() const
	{
		return min(x, y);
	}

	inline double maxValue() const
	{
		return max(x, y);
	}

	inline const doublePoint normalized(double min, double d)
	{
		return doublePoint((x - min) / d, (y - min) / d);
	}
};
