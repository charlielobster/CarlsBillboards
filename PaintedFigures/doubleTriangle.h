#pragma once

#include "doublePoint.h"
#include "doubleLine.h"

class doubleTriangle
{
public:
	doublePoint vertices[3];
	doubleTriangle() {}

	doubleTriangle(const doubleTriangle &t) 
	{
		vertices[0] = t.vertices[0];
		vertices[1] = t.vertices[1];
		vertices[2] = t.vertices[2];
	}

	doubleTriangle(const doublePoint &a, const doublePoint &b, const doublePoint &c) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}

	inline doubleTriangle &operator=(const doubleTriangle &l)
	{
		if (this == &l) return *this;
		vertices[0] = l.vertices[0]; 
		vertices[1] = l.vertices[1]; 
		vertices[2] = l.vertices[2];
		return *this;
	}

	inline doublePoint &operator[](int i) { return vertices[i]; }
};


