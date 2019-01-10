#pragma once

#include <assert.h>
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
	double m;
	double d;
	double dot;
	double min;
	double max;
	doublePoint() {}

	void set()
	{
		m = y / x;
		min = min(x, y); 
		max = max(x, y);
		dot = x * x + y * y;
		d = sqrt(dot);
	}

	doublePoint(double d) : x(d), y(d) 
	{
		set();
	}

	doublePoint(const doublePoint &p) : x(p.x), y(p.y) 
	{
		set();
	}

	doublePoint(double x, double y) : x(x), y(y) 
	{
		set();
	}

	const doublePoint &operator=(const doublePoint &p)
	{ 
		if (this == &p) return *this; 
		x = p.x; y = p.y; set();
		return *this;
	}

	const double &operator[](unsigned short i) const { assert(i < 2);  return (i == 0 ? x : y); }

	friend const doublePoint operator-(const doublePoint &q, const doublePoint &p)
	{
		return doublePoint(q.x - p.x, q.y - p.y);
	}

	friend const doublePoint operator-(const doublePoint &p, double d)
	{
		return doublePoint(p.x - d, p.y - d);
	}

	friend const doublePoint operator/(const doublePoint &p, double d)
	{
		assert(d != 0);
		return doublePoint(p.x / d, p.y / d);
	}

	friend const doublePoint operator*(const doublePoint &p, double d)
	{
		return doublePoint(p.x * d, p.y * d);
	}

	friend const doublePoint operator+(const doublePoint &p, const doublePoint &q)
	{
		return doublePoint(p.x + q.x, p.y + q.y);
	}

	friend const doublePoint operator+(const doublePoint &p, double d)
	{
		return doublePoint(p.x + d, p.y + d);
	}

	friend double cross(const doublePoint &p, const doublePoint &q)
	{
		return (q.y * p.x - q.x * p.y);
	}

	friend double dot(const doublePoint &p, const doublePoint &q)
	{
		return (q.x * p.x + q.y * p.y);
	}
};
