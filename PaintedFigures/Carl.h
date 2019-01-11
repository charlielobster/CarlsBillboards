#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <vector>

#include "doublePoint.h"
#include "doubleLine.h"
#include "doubleTriangle.h"
#include "doubleRect.h"
#include "capsule.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_RADIUS 49
#define MAX_DIMENSION 999
#define MAX_N 49

/*
Carl's Algorithm:

Carl collects all the capsules
Carl first breaks each capsule into two triangles making up a rect of size radius x line.distance, 
and two semi-circles on either ends of the rect

Carl handles triangles first:

	Carl puts all the triangles making up all the capsules into his "bag"
	he also makes a new empty collection of triangles, the "fragments" that have already been placed into the billboard
	the bag can have many duplicates of what is in fragments,  
	but every triangle in fragments is unique in that no 2 triangles in fragments share any area
	but triangles in fragments may share vertices or have co-linear sides
	
	while Carl finds a triangle b in his bag

		while Carl finds a triangle f in fragments
			
			Carl checks to see if b is completely enclosed by f
				if b is completely surrounded by f, Carl throws b away
				Carl breaks out and gets another b from the bag
				
			Carl checks to see if f is completely enclosed by b
				if f is completely surrounded by b, then Carl breaks up b in the following way:
				create 3 vertices at b's midpoints and along with b and f's vertices, break up b into 10 triangles 
				Carl puts the 9 triangles back into his bag and throws out the left-over copy of f 
				Carl breaks out and gets another b from the bag

			Carl checks to see if f and b completely miss one another
				if f and b completely miss each other, Carl breaks out and gets another f from fragments
			
			Carl checks to see if b intersects f, the last possibility
				if f and b intersect, and since b and f can share vertices and edges, 
				their edges must either intersect at 2 or 4 points
				if only at 2 points, Carl makes one cut into b at those points
				if their edges intersect at 4 points, then Carls makes two pairs of cuts into b,	
					Carl then tosses out the piece of b that overlapped with f
					This leaves Carl with triangles, quadrilaterals, or a 5 or 6-sided dimpled shape -
					in case of the 4 or more sided shapes, Carl further breaks those into triangles, 
						using special tricks he has figured out for each class of shape
					Carl then throws all the triangles he created back into the bag
					then Carl breaks out and gets a new b from his bag
				
		if Carl finishes checking every f in fragments and he is still holding b, 
			Carl adds b to fragments and gets a new b from his bag

	if there are no triangles left in the bag, continue to circles...	

*/

using namespace std;

struct intTriangle
{
	Point a;
	Point b;
	Point c;
};

class Carl
{
public:
	capsule capsules[200];
	capsule unitCapsules[200];

	vector<doubleTriangle> bag;			// all the triangles Carl wants to paint
	vector<doubleTriangle> fragments;	// the triangle fragments comprising the colored regions of Carl's billboard


	vector<doubleLine> screenLines;
	vector<doubleRect> screenRects;
	vector<doubleTriangle> unitBag;
	vector<doubleTriangle> screenBag;
	vector<doubleTriangle> screenTiles;
	int tileCount = 0;

	unsigned short n;
	double min = MAX_DOUBLE, max = MIN_DOUBLE, d = 0;

	//bool sameSide(const doublePoint &p1, const doublePoint &p2, const doublePoint &a, const doublePoint &b)
	//{
	//	double c = cross((b - a), (p1 - a));
	//	double cp1 = crossProduct(b - a, p1 - a);
	//	double cp2 = crossProduct(b - a, p2 - a);
	//	if (cp1 * cp2 >= 0.0) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	//bool pointInTriangle(doublePoint &p, doubleTriangle &t)
	//{
	//	if (sameSide(p, t[0], t[1], t[2]) &&
	//		sameSide(p, t[1], t[0], t[2]) &&
	//		sameSide(p, t[2], t[0], t[1])) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	//doublePoint linesIntersect(const doublePoint &p, const doublePoint &q)
	//{
	//	doublePoint d = q - p;
	//	double m = d.y / d.x;
	//	double b = m * p.x - p.y;
	//}
	//// Given three colinear points p, q, r, the function checks if 
	//// point q lies on line segment 'pr' 
	//bool onSegment(doublePoint p, doublePoint q, doublePoint r)
	//{
	//	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
	//		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
	//		return true;
	//	return false;
	//}
	//// To find orientation of ordered triplet (p, q, r). 
	//// The function returns following values 
	//// 0 --> p, q and r are colinear 
	//// 1 --> Clockwise 
	//// 2 --> Counterclockwise 
	//int orientation(doublePoint p, doublePoint q, doublePoint r)
	//{
	//	// for details of below formula. 
	//	int val = (q.y - p.y) * (r.x - q.x) -
	//		(q.x - p.x) * (r.y - q.y);
	//	if (val == 0) return 0;  // colinear 
	//	return (val > 0) ? 1 : 2; // clock or counterclock wise 
	//}
	//// The main function that returns true if line segment 'p1q1' 
	//// and 'p2q2' intersect. 
	//bool doIntersect(doublePoint p1, doublePoint q1, doublePoint p2, doublePoint q2)
	//{
	//	// Find the four orientations needed for general and 
	//	// special cases 
	//	int o1 = orientation(p1, q1, p2);
	//	int o2 = orientation(p1, q1, q2);
	//	int o3 = orientation(p2, q2, p1);
	//	int o4 = orientation(p2, q2, q1);
	//	// General case 
	//	if (o1 != o2 && o3 != o4)
	//		return true;
	//	// Special Cases 
	//	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	//	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	//	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	//	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	//	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	//	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	//	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	//	if (o4 == 0 && onSegment(p2, q1, q2)) return true;
	//	return false; // Doesn't fall in any of the above cases 
	//}
	////// Driver program to test above functions 
	////int main()
	////{
	////	struct Point p1 = { 1, 1 }, q1 = { 10, 1 };
	////	struct Point p2 = { 1, 2 }, q2 = { 10, 2 };
	////	doIntersect(p1, q1, p2, q2) ? cout << "Yes\n" : cout << "No\n";
	////	p1 = { 10, 0 }, q1 = { 0, 10 };
	////	p2 = { 0, 0 }, q2 = { 10, 10 };
	////	doIntersect(p1, q1, p2, q2) ? cout << "Yes\n" : cout << "No\n";
	////	p1 = { -5, -5 }, q1 = { 0, 0 };
	////	p2 = { 1, 1 }, q2 = { 10, 10 };
	////	doIntersect(p1, q1, p2, q2) ? cout << "Yes\n" : cout << "No\n";
	////	return 0;
	////}
	//bool trianglesIntersect(int i, int j)
	//{
	//	// determine if co-planar triangles triangles[i] intersects (triangle) tiles[j]
	//	bool result = false;
	//	unsigned short k = 0;
	//	//while (!result && k < 3) {
	//	//	doublePoint p = triangles[i][k];
	//	//	result = pointInTriangle(p, tiles[j]);
	//	//	k++;
	//	//}
	//	//k = 0;
	//	while (!result && k < 3) {
	//		doublePoint p1 = bag[i][k];
	//		doublePoint q1 = bag[i][(k + 1) % 3];
	//		unsigned short l = 0;
	//		while (!result && l < 3) {
	//			doublePoint p2 = fragments[j][l];
	//			doublePoint q2 = fragments[j][(l + 1) % 3];
	//			result = doIntersect(p1, q1, p2, q2);
	//			l++;
	//		}
	//		k++;
	//	}
	//	return result;
	//}

	void init()
	{
		//srand((unsigned int)time(0));
		n = 3; // (rand() % MAX_N) + 1;

		for (int i = 0; i < n; i++) {
			capsules[i] = capsule(
				doubleLine(
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1),
				rand() % MAX_RADIUS + 10);
			if (capsules[i].min < min) { min = capsules[i].min; }
			if (capsules[i].max > max) { max = capsules[i].max; }
			doubleRect r = capsules[i].rect;
			doubleTriangle t1(r.a, r.b, r.c);
			doubleTriangle t2(r.b, r.c, r.d);
			bag.push_back(t1);
			bag.push_back(t2);
		}
		min -= MAX_RADIUS; max += MAX_RADIUS;
		d = max - min;

		for (int i = 0; i < n; i++) {
			unitCapsules[i] = (capsules[i] - min) / d;
		}

		int s = bag.size();
		for (int i = 0; i < s; i++) {
			unitBag.push_back((bag[i] - min) / d);
		}

		//for (int i = 1; i < 2 * n; i++) {
		//	bool result = false;
		//	unsigned short j = 0;
		//	while (!result && (j < tileCount)) {
		//		result = trianglesIntersect(i, j);
		//		j++;
		//	}
		//	if (!result) {
		//		fragments.push_back(bag[i]);
		//		tileCount++;
		//	}
		//}
		//screenTiles.resize(tileCount);
	}

	void resize(double minScreen)
	{
		screenRects.clear();
		screenLines.clear();
		screenBag.clear();
		for (int i = 0; i < n; i++) {
			doubleRect r = unitCapsules[i].rect * minScreen;
			screenRects.push_back(r);
			doubleLine l = unitCapsules[i].line * minScreen;
			screenLines.push_back(l);
		}

		int s = unitBag.size();
		for (int i = 0; i < s; i++) {
			doubleTriangle t = unitBag[i] * minScreen;
			screenBag.push_back(t);
		}

		//for (unsigned short i = 0; i < tileCount; i++) {
		//	screenTriangles[i][0].x = triangles[i][0].x * minScreen;
		//	screenTriangles[i][0].y = triangles[i][0].y * minScreen;
		//	screenTriangles[i][1].x = triangles[i][1].x * minScreen;
		//	screenTriangles[i][1].y = triangles[i][1].y * minScreen;
		//	screenTriangles[i][2].x = triangles[i][2].x * minScreen;
		//	screenTriangles[i][2].y = triangles[i][2].y * minScreen;
		//}
		//for (int i = 0; i < tileCount; i++) {
		//	screenTiles[i][0].x = tiles[i][0].x * minScreen;
		//	screenTiles[i][0].y = tiles[i][0].y * minScreen;
		//	screenTiles[i][1].x = tiles[i][1].x * minScreen;
		//	screenTiles[i][1].y = tiles[i][1].y * minScreen;
		//	screenTiles[i][2].x = tiles[i][2].x * minScreen;
		//	screenTiles[i][2].y = tiles[i][2].y * minScreen;
		//}
	}

	void fillRect(Graphics &g, int i)
	{
		Point r0(screenRects[i][0][0], screenRects[i][0][1]);
		Point r1(screenRects[i][1][0], screenRects[i][1][1]);
		Point r2(screenRects[i][2][0], screenRects[i][2][1]);
		Point r3(screenRects[i][3][0], screenRects[i][3][1]);

		SolidBrush b(Color(100, 140, 255));
		GraphicsPath path;
		path.AddLine(r0, r1);
		path.AddLine(r1, r3);
		path.AddLine(r3, r2);
		path.AddLine(r2, r0);
		g.FillPath(&b, &path);
	}

	void drawRect(Graphics &g, int i)
	{
		Point r0(screenRects[i][0][0], screenRects[i][0][1]);
		Point r1(screenRects[i][1][0], screenRects[i][1][1]);
		Point r2(screenRects[i][2][0], screenRects[i][2][1]);
		Point r3(screenRects[i][3][0], screenRects[i][3][1]);

		Pen p(Color(0, 255, 0));
		GraphicsPath path;
		path.AddLine(r0, r1);
		path.AddLine(r1, r3);
		path.AddLine(r3, r2);
		path.AddLine(r2, r0);
		g.DrawPath(&p, &path);
	}

	void drawTiles(Graphics &g, int i)
	{
		Pen p(Color(255, 0, 0));
		GraphicsPath path;
		Point a(screenTiles[i][0].x, screenTiles[i][0].y);
		Point b(screenTiles[i][1].x, screenTiles[i][1].y);
		Point c(screenTiles[i][2].x, screenTiles[i][2].y);
		path.AddLine(a, b);
		path.AddLine(b, c);
		path.AddLine(c, a);
		g.DrawPath(&p, &path);
	}

	void drawBag(Graphics &g, int i)
	{
		Pen p(Color(255, 0, 0));
		GraphicsPath path;
		Point a(screenBag[i].a.x, screenBag[i].a.y);
		Point b(screenBag[i].b.x, screenBag[i].b.y);
		Point c(screenBag[i].c.x, screenBag[i].c.y);
		path.AddLine(a, b);
		path.AddLine(b, c);
		path.AddLine(c, a);
		g.DrawPath(&p, &path);
	}

	void drawRectEndPoints(Graphics &g, int i)
	{
		Pen p(Color(255, 0, 0), 4);
		g.DrawEllipse(&p, screenRects[i][0][0] - 2, screenRects[i][0][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenRects[i][1][0] - 2, screenRects[i][1][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenRects[i][2][0] - 2, screenRects[i][2][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenRects[i][3][0] - 2, screenRects[i][3][1] - 2, 4, 4);
	}

	void paintLine(Graphics &g, int i)
	{
		Pen pen(Color(255, 255, 255), 1);
		Point p0(screenLines[i][0][0], screenLines[i][0][1]);
		Point p1(screenLines[i][1][0], screenLines[i][1][1]);
		g.DrawLine(&pen, p0, p1);
	}

	void paintLineEndPoints(Graphics &g, int i)
	{
		Pen p(Color(0, 0, 0), 6);
		g.DrawEllipse(&p, screenLines[i][0][0] - 3, screenLines[i][0][1] - 3, 6, 6);
		g.DrawEllipse(&p, screenLines[i][1][0] - 3, screenLines[i][1][1] - 3, 6, 6);
	}

	void paint(Graphics &g)
	{
		for (int i = 0; i < n; i++) {
			fillRect(g, i);
		}
		//for (int i = 0; i < tileCount; i++) {
		//	drawTiles(g, i);
		//}
		int s = bag.size();
		for (int i = 0; i < s; i++) {
			drawBag(g, i);
		}
		//for (int i = 0; i < n; i++) {
		//	drawRect(g, i);
		//}
		for (int i = 0; i < n; i++) {
			drawRectEndPoints(g, i);
		}
		//for (int i = 0; i < n; i++) {
		//	paintLine(g, i);
		//}
		for (int i = 0; i < n; i++) {
			paintLineEndPoints(g, i);
		}
	}
};

