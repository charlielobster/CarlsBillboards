#pragma once

#include "doublePoint.h"
#include "doubleLine.h"

struct sharedItems
{
	int indices[3];
	int hits;
	int first;
	sharedItems() : hits(0) , first(-1)
	{
		for (unsigned short i = 0; i < 3; i++) {
			indices[i] = -1;
		}
	}
};

class doubleTriangle
{
public:
	doublePoint a;
	doublePoint b;
	doublePoint c;
	doubleLine ab;
	doubleLine bc;
	doubleLine ca;
	double area;
	doubleTriangle() {}

	void set()
	{
		ab = doubleLine(a, b); 
		bc = doubleLine(b, c); 
		ca = doubleLine(c, a);

		// Carl read about Heron's Formula for finding the area of a triangle:
		// https://en.wikipedia.org/wiki/Heron%27s_formula
		double s = .5 * (ab.d + bc.d + ca.d);
		area = sqrt( s * (s - ab.d ) * (s - bc.d) * (s - ca.d) );
	}

	doubleTriangle(const doubleTriangle &t) : a(t.a), b(t.b), c(t.c)
	{
		set();
	}

	doubleTriangle(const doublePoint &ta, const doublePoint &tb, const doublePoint &tc) : a(ta), b(tb), c(tc)
	{
		set();
	}

	doubleTriangle &operator=(const doubleTriangle &l)
	{
		if (this == &l) return *this;
		a = l.a; b = l.b; c = l.c; 
		set();
		return *this;
	}

	const doublePoint &vertex(unsigned short i) const 
	{
		assert(i < 3); 
		switch (i) { 
			case 0: return a; 
			case 1: return b; 
			default: return c;
		}
	}

	const doubleLine &edge(unsigned short i) const
	{
		assert(i < 3);
		switch (i) {
			case 0: return ab;
			case 1: return bc;
			default: return ca;
		}
	}

	friend bool operator==(const doubleTriangle &s, const doubleTriangle &t)
	{
		return (s.a == t.a && s.b == t.b && s.c == t.c);
	}

	friend const doubleTriangle operator-(const doubleTriangle &t, double d)
	{
		return doubleTriangle(t.a - d, t.b - d, t.c - d);
	}

	friend const doubleTriangle operator/(const doubleTriangle &t, double d)
	{
		assert(d != 0);
		return doubleTriangle(t.a / d, t.b / d, t.c / d);
	}

	friend const doubleTriangle operator*(const doubleTriangle &t, double d)
	{
		return doubleTriangle(t.a * d, t.b * d, t.c * d);
	}

	friend sharedItems getSharedEdges(const doubleTriangle &s, const doubleTriangle &t)
	{
		sharedItems result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (colinear(s.edge(i), t.edge(j))) {
					result.first = i;
					result.indices[i] = j;
					result.hits = 1;
					return result;
				}
			}
		}
		return result;
	}

	friend sharedItems getSharedVertices(const doubleTriangle &s, const doubleTriangle &t)
	{
		sharedItems result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (s.vertex(i) == t.vertex(j)) {
					if (result.first == -1) { result.first = i; }
					result.indices[i] = j;
					result.hits++;
				}
			}
		}
		return result;
	}
};


