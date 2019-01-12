#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <list>

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

typedef enum 
{ 
	SAME,
	CO_LINEAR_OVERLAP,
	CO_LINEAR_TOUCHING,
	B_SURROUNDS_F,
	F_SURROUNDS_B, 
	COMPLETE_MISS, 
	INTERSECTION 
} TRIANGLE_RELATION;

/*
Carl's Billboards Algorithm:

Carl collects all the capsules he wants to use for this billboard, putting each one into his bag, one at a time.
As he inserts them, Carl sorts the capsules by size, with the largest being first. 

Once all the capsules are collected (and sorted by size in the bag), Carl begins placing capsules, 

Starting with the largest capsule in the bag, Carl places a capsule on the billboard.
He goes through all the capsules, one at a time, always starting with the largest capsule, until he reaches the last capsule.

If Carl 

his only rule at this point is to only place capsules that do not overlap


Carl does so by breaking up the capsule into two triangles and two semi-circles.


is only place the capsule by breaking the capsule into two triangles and two semi-circles.
He uses 
Carl uses the 

Carl first breaks each capsule into two triangles making up a rect of size radius x line.distance, 
and two semi-circles on either ends of the rect

Carl handles triangles next:

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

class Carl
{
public:
	//capsule capsules[200];
	//capsule unitCapsules[200];

	list<capsule>capsules;
	list<capsule>unitCapsules;

	list<doubleTriangle> bag;			// the triangles Carl wants to paint
	vector<doubleTriangle> fragments;	// the triangular fragments (colored regions) on Carl's billboard

	list<doubleTriangle> unitBag;
	vector<doubleTriangle> unitFragments;

	vector<doubleLine> screenLines;
	vector<doubleRect> screenRects;
	
	list<doubleTriangle> screenBag;
	vector<doubleTriangle> screenFragments;

	unsigned short n;
	double min = MAX_DOUBLE, max = MIN_DOUBLE, d = 0;

	//bool sameSide(const doublePoint &p, const doublePoint &q, const doublePoint &a, const doublePoint &b)
	//{
	//	double c = cross((b - a), (p - a));
	//	double cp1 = crossProduct(b - a, p - a);
	//	double cp2 = crossProduct(b - a, q - a);
	//	if (cp1 * cp2 >= 0.0) {
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	//bool pointInTriangle(doublePoint &p, doubleTriangle &t)
	//{
	//	if (sameSide(p, t.a, t.b, t.c) &&
	//		sameSide(p, t.b, t.a, t.c) &&
	//		sameSide(p, t.b, t.a, t.b)) {
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

	TRIANGLE_RELATION getTriangleRelation(const doubleTriangle &b, const doubleTriangle &f)
	{
		if (b == f) return SAME;

		sharedItems e, v = getSharedVertices(b, f);	
		unsigned short sourceIndex, targetIndex, nextSourceIndex, nextTargetIndex;
		doubleLine sourceEdge, targetEdge, nextSourceEdge, nextTargetEdge;
		double cs, ct;

		// Carl finds either 2, 1 or 0 shared vertices
		assert(v.hits >= 0 && v.hits <= 2);

		switch (v.hits) {
		case 2:
			// Carl found 2 co-linear triangles

			e = getSharedEdges(b, f);
			// Carl can only find 1 shared edge
			assert(e.hits == 1);

			// For b and f, Carl finds the cross product of each triangle's 
			// shared edge with the next edge in the triangle.
			// If the cross products are the same sign, then the triangles must overlap.
			sourceIndex = e.first;
			nextSourceIndex = (sourceIndex + 1) % 3;

			targetIndex = e.indices[sourceIndex];
			nextTargetIndex = (targetIndex + 1) % 3;

			sourceEdge = b.edge(sourceIndex);
			nextSourceEdge = b.edge(nextSourceIndex);

			targetEdge = f.edge(targetIndex);
			nextTargetEdge = f.edge(nextTargetIndex);

			cs = cross(sourceEdge.pq, nextSourceEdge.pq);
			ct = cross(targetEdge.pq, nextTargetEdge.pq);

			// if the normalized shared edge is going in different directions for b and f, Carl flips f's cross product's sign
//			if (sourceEdge != targetEdge) ct *= -1.0;

			if (cs * ct > 0.0) {
				return CO_LINEAR_OVERLAP;
			} else {
				return CO_LINEAR_TOUCHING;
			}

		case 1:
			break;

		case 0:

			// Although no edges or vertices match, the triangles may still overlap...
			// Their edges can either intersect at 2 or 4 points
			// Carl find some intersection points
			return INTERSECTION;
			// else 
			return COMPLETE_MISS;
		}
//		if (e.hits) {
//			// Carl found a co-linear triangle
//			// if the directions of the cross products are the same sign, then the triangles must overlap
//			doubleLine l;
//			doublePoint p, q;
//			if (v.targetEdge == AB) {
//
//			}
////			doubleLine s = (v == AB ? b.ab : (v == BC ? b.bc : b.ca));
//			return CO_LINEAR_OVERLAP;
//
//			return CO_LINEAR_TOUCHING;
//		} else {
			// Carl also checks to see if the triangle edges intersect anywhere
//		}
//		if (v.sourceEdge == AB || v.sourceEdge == BC || v.sourceEdge == CA) {
	}

	void fillFragments()
	{
		//	Carl has already put the triangles making up all the capsules into his "bag", 
		//	which is sorted by area, with the largest first.
		//	Carl also makes a new empty collection of triangles, the "fragments", 
		//	which are triangles already placed on the billboard.
		//	The bag can have many duplicates of what is in fragments. 
		//	However, every triangle in fragments is unique, in that no 2 triangles in fragments share any area, 
		//	but triangle fragments may share vertices or have co-linear sides

		//	while Carl finds a triangle b in his bag
		list<doubleTriangle>::iterator b = bag.begin();
		doubleTriangle f;
		while (b != bag.end()) {

			//	while Carl finds a triangle f in fragments
			int i = 0, s = fragments.size();
			bool breakout = false;
			while (!breakout && i < s) {

				f = fragments[i++];
				TRIANGLE_RELATION state = getTriangleRelation(*b, f);
				switch (state) {

				case SAME:
				case F_SURROUNDS_B:
					//  Carl checks to see if b is completely enclosed by f
					//  	if b is completely surrounded by f, Carl throws b away
					//  	Carl breaks out and gets another b from the bag
					++b;
					breakout = true;
					break;

				case B_SURROUNDS_F:
					//  Carl checks to see if f is completely enclosed by b
					//  	if f is completely surrounded by b, then Carl breaks up b in the following way:
					//  	create 3 vertices at b's midpoints and along with b and f's vertices, break up b into 10 triangles
					//  	Carl puts the 9 triangles back into his bag and throws out the left-over copy of f
					//  	Carl breaks out and gets another b from the bag
					++b;
					breakout = true;
					break;

				case CO_LINEAR_TOUCHING:
				case COMPLETE_MISS:
					//  Carl checks to see if f and b completely miss one another
					//  	if f and b completely miss each other, Carl gets another f from fragments
					break;

				case CO_LINEAR_OVERLAP:
				case INTERSECTION:
					//  Carl checks to see if b intersects f, the last possibility
					//  	if f and b intersect, and since b and f can share vertices and edges,
					//  	their edges must either intersect at 2 or 4 points
					//  	if only at 2 points, Carl makes one cut into b at those points
					//  	if their edges intersect at 4 points, then Carls makes two pairs of cuts into b,
					//  		Carl then tosses out the piece of b that overlapped with f
					//  		This leaves Carl with triangles, quadrilaterals, or a 5 or 6-sided dimpled shape -
					//  		in case of the 4 or more sided shapes, Carl further breaks those into triangles,
					//  			using special tricks he has figured out for each class of shape
					//  		Carl then throws all the triangles he created back into the bag
					//  		then Carl breaks out and gets a new b from his bag
					++b;
					breakout = true;
					break;
				}
			}

			//	if Carl finishes checking every f in fragments and he is still holding b,
			//		Carl adds b to fragments and gets a new b from his bag
			fragments.push_back(*b);
			++b;
		}

		//if there are no triangles left in the bag, continue to circles...
	}

	void insertCapsule(list<capsule> &capsules, const capsule &c)
	{
		list<capsule>::iterator cptr = capsules.begin();
		while (cptr != capsules.end() && cptr->area > c.area) {
			++cptr;
		}
		capsules.insert(cptr, c);
	}

	void init()
	{
		capsule c;
		//srand((unsigned int)time(0));
		n = 3; // (rand() % MAX_N) + 1;

		for (int i = 0; i < n; i++) {
			// Carl collects a capsule
			c = capsule(
				doubleLine(
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1,
					rand() % MAX_DIMENSION + 1),
				rand() % MAX_RADIUS + 10);
			insertCapsule(capsules, c);

			// Carl also collects the capsule's bounds
			if (c.min < min) { min = c.min; }
			if (c.max > max) { max = c.max; }
		}

		list<capsule>::iterator cptr = capsules.begin();
		while (cptr != capsules.end()) {
			// Carl adds the capsule's triangles to his bag
			doubleRect r = cptr->rect;
			doubleTriangle t1(r.a, r.b, r.c);
			doubleTriangle t2(r.b, r.c, r.d);
			bag.push_back(t1);
			bag.push_back(t2);
			++cptr;
		}

		// Carl has filled his bag of triangles
		// he's ready to sort the contents into billboard fragments
		fillFragments();

		// find the overall bounds of the set 
		// use the results to scale Carl's objects to the window
		min -= MAX_RADIUS; max += MAX_RADIUS;
		d = max - min;

		cptr = capsules.begin();
		while (cptr != capsules.end()) {
			insertCapsule(unitCapsules, (*cptr - min) / d);
			++cptr;
		}

		list<doubleTriangle>::iterator b = bag.begin();
		while (b != bag.end()) {
			unitBag.push_back((*b - min) / d);
			++b;
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
		screenFragments.clear();

		list<capsule>::iterator cptr = unitCapsules.begin();
		while (cptr != unitCapsules.end()) {
			doubleRect r = cptr->rect * minScreen;
			screenRects.push_back(r);
			doubleLine l = cptr->line * minScreen;
			screenLines.push_back(l);
			++cptr;
		}

		list<doubleTriangle>::iterator u = unitBag.begin();
		while (u != unitBag.end()) {
			screenBag.push_back(*u * minScreen);
			++u;
		}

		//int v = unitFragments.size();
		//for (int i = 0; i < v; i++) {
		//	doubleTriangle t = unitFragments[i] * minScreen;
		//	screenFragments.push_back(t);
		//}

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

	void fillRects(Graphics &g)
	{
		doubleRect r;
		for (int i = 0; i < n; i++) {
			r = screenRects[i];
			Point a(r.a.x, r.a.y);
			Point b(r.b.x, r.b.y);
			Point c(r.c.x, r.c.y);
			Point d(r.d.x, r.d.y);
			SolidBrush brush(Color(100, 140, 255));
			GraphicsPath path;
			path.AddLine(a, b);
			path.AddLine(b, d);
			path.AddLine(d, c);
			path.AddLine(c, a);
			g.FillPath(&brush, &path);
		}
	}

	void drawRects(Graphics &g)
	{
		Pen pen(Color(0, 255, 0));
		doubleRect r;
		for (int i = 0; i < n; i++) {
			r = screenRects[i];
			Point a(r.a.x, r.a.y);
			Point b(r.b.x, r.b.y);
			Point c(r.c.x, r.c.y);
			Point d(r.d.x, r.d.y);
			GraphicsPath path;
			path.AddLine(a, b);
			path.AddLine(b, d);
			path.AddLine(d, c);
			path.AddLine(c, a);
			g.DrawPath(&pen, &path);
		}
	}

	void drawFragments(Graphics &g, int i)
	{
		Pen pen(Color(255, 0, 0));
		GraphicsPath path;
		Point a(screenFragments[i].a.x, screenFragments[i].a.y);
		Point b(screenFragments[i].b.x, screenFragments[i].b.y);
		Point c(screenFragments[i].c.x, screenFragments[i].c.y);
		path.AddLine(a, b);
		path.AddLine(b, c);
		path.AddLine(c, a);
		g.DrawPath(&pen, &path);
	}

	void paintBag(Graphics &g)
	{
		Pen pen(Color(255, 0, 0));
		list<doubleTriangle>::iterator sb = screenBag.begin();
		while (sb != screenBag.end()) {
			GraphicsPath path;
			Point a(sb->a.x, sb->a.y);
			Point b(sb->b.x, sb->b.y);
			Point c(sb->c.x, sb->c.y);
			path.AddLine(a, b);
			path.AddLine(b, c);
			path.AddLine(c, a);
			g.DrawPath(&pen, &path);
			++sb;
		}
	}

	void paintRectEndPoints(Graphics &g)
	{
		Pen pen(Color(255, 0, 0), 4);
		doubleRect r;
		for (int i = 0; i < n; i++) {
			r = screenRects[i];
			g.DrawEllipse(&pen, r.a.x - 2, r.a.y - 2, 4, 4);
			g.DrawEllipse(&pen, r.b.x - 2, r.b.y - 2, 4, 4);
			g.DrawEllipse(&pen, r.c.x - 2, r.c.y - 2, 4, 4);
			g.DrawEllipse(&pen, r.d.x - 2, r.d.y - 2, 4, 4);
		}
	}

	void paintLines(Graphics &g)
	{
		Pen pen(Color(255, 255, 255), 1);
		doubleLine l;
		for (int i = 0; i < n; i++) {
			l = screenLines[i];
			Point p(l.p.x, l.p.y);
			Point q(l.q.x, l.q.y);
			g.DrawLine(&pen, p, q);
		}
	}

	void paintLineEndPoints(Graphics &g)
	{
		Pen pen(Color(0, 0, 0), 6);
		doubleLine l;
		for (int i = 0; i < n; i++) {
			l = screenLines[i];
			g.DrawEllipse(&pen, l.p.x - 3, l.p.y - 3, 6, 6);
			g.DrawEllipse(&pen, l.q.x - 3, l.q.y - 3, 6, 6);
		}
	}

	void paint(Graphics &g)
	{
		fillRects(g);
		//int s = fragments.size();
		//for (int i = 0; i < s; i++) {
		//	drawFragments(g, i);
		//}
		paintBag(g);
		drawRects(g);
		paintRectEndPoints(g);
		paintLines(g);
		paintLineEndPoints(g);
	}
};

