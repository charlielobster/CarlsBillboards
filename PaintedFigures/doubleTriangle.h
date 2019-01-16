#pragma once

#include <list>

#include "doublePoint.h"
#include "doubleLine.h"

using namespace std;

struct triangleIntersectionPoint
{
	unsigned short sourceIndex;
	unsigned short targetIndex;
	doublePoint point;
	triangleIntersectionPoint(
		unsigned short source, 
		unsigned short target, 
		const doublePoint &pt) : 
		sourceIndex(source), 
		targetIndex(target), 
		point(pt) {}
	triangleIntersectionPoint(const doublePoint &pt) : 
		triangleIntersectionPoint(0, 0, pt) {}
	triangleIntersectionPoint() : 
		triangleIntersectionPoint(MIN_POINT) {}
};

struct triangleIntersections
{
	triangleIntersectionPoint pts[4];
	int hits;
	triangleIntersections() : hits(0) {}
};

struct triangleQuery
{
	int indices[3];
	int hits;
	int first;
	triangleQuery() : hits(0) , first(-1)
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
	doublePoint minPoint;
	doublePoint maxPoint;
	doubleRect bounds;
	double min;
	double max;
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

		minPoint = doublePoint(min(a.x, min(b.x, c.x)), min(a.y, min(b.y, c.y)));
		maxPoint = doublePoint(max(a.x, max(b.x, c.x)), max(a.y, max(b.y, c.y)));
		bounds = doubleRect(
			minPoint,
			doublePoint(minPoint.x, maxPoint.y),
			maxPoint,
			doublePoint(maxPoint.x, minPoint.y)
		);
		min = min(minPoint.x, minPoint.y);
		max = max(maxPoint.x, maxPoint.y);
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

	friend triangleQuery getSharedEdges(const doubleTriangle &s, const doubleTriangle &t)
	{
		triangleQuery result;
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

	friend triangleQuery getSharedVertices(const doubleTriangle &s, const doubleTriangle &t)
	{
		triangleQuery result;
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

	friend void insertTriangle(list<doubleTriangle> &triangles, const doubleTriangle &t)
	{
		list<doubleTriangle>::iterator tptr =  triangles.begin();
		while (tptr != triangles.end() && tptr->area > t.area) {
			++tptr;
		}
		triangles.insert(tptr, t);
	}

	friend triangleQuery boundVertices(const doubleTriangle &l, const doubleTriangle &r)
	{
		triangleQuery q;
		for (int i = 0; i < 3; i++) {
			if (r.bounds.contains(l.vertex(i))) {
				q.indices[i] = 1; q.hits++; if (q.first == -1) q.first = i;
			}
		}
		return q;
	}
};


