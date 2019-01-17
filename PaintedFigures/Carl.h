#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <list>
#include <assert.h>

#include "doublePoint.h"
#include "doubleLine.h"
#include "doubleTriangle.h"
#include "doubleRect.h"
#include "capsule.h"
#include "windowsHelper.h"

#define MAX_RADIUS 49
#define MAX_DIMENSION 999
#define MAX_N 49

typedef enum
{
	DISCARD,
	B_SURROUNDS_F, // Carl is not sure if this state is possible...
	MISS,
	INTERSECTION // Carl wonders if this is too general a state
} triangleRelation; // Carl wonders if this enum is all that useful...

/*
	Carl's Billboards Algorithm:

	Carl only paints 2D versions of something called a "capsule". 
	There is a 3D version of the kind of figure Carl paints described here:
	https://en.wikipedia.org/wiki/Capsule_%28geometry%29 
	Carl thinks of it as linear strokes of a monochromatic pen with variable thickness.
	They look like shadows of capsules, or tic-tacs, or a special kind of ovals, called "Cassini" ovals...

	At any rate, Carl breaks the figure, or capsule, 
	into 2 triangles that make up 1 rectangle, and 2 semi-circles on either end of the rectangle.
	Carl puts the semi-circles aside and deals with them later.
	Carl inserts the triangles making up the rectangle into his "bag".
	As he does so, he sorts them by area, with the largest always being first out of his bag.

	Once all the capsules are collected (and triangles making up the rectangular portions of the capsule sorted by area in the bag),
	Carl begins placing triangles into his empty "fragments" collection.
	Carl's "fragments" are triangles that, in a sense, have already been placed on the billboard.
	The bag can have many duplicates of what is inside it or in fragments.
	But every triangle in fragments is "unique" in that no 2 triangles in fragments share any area.
	However, fragments may share vertices, or even have co-linear or partially co-linear sides.


	Carl fills his fragments collection:
	while Carl finds a triangle b in his bag  -- outer loop

		while Carl finds a triangle f in fragments	-- inner loop
			
			Carl checks to see if b is completely enclosed by f
				if b is completely surrounded by f, Carl throws b away
					Carl breaks out of inner loop and gets another b from the bag
				
			Carl checks to see if f is completely enclosed by b
				(Carl isn't sure this is even a valid case, because he starts out with a bag sorted by area with largest first...)
				Carl supposes that if this case occurs and f is completely surrounded by b, then Carl will break up b in the following way:
					Create 3 vertices at b's midpoints and along with b and f's vertices, break up b into 10 triangles 
					Carl puts the 9 triangles back into his bag and throws out the left-over copy of f 
					Carl breaks out of inner loop and gets another b from the bag

			Carl checks to see if f and b completely miss one another
				If f and b completely miss each other, Carl gets another f from fragments
			
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
	list<capsule>capsules;
	list<doubleTriangle> bag;			// the triangles Carl wants to paint
	vector<doubleTriangle> fragments;	// the triangular fragments (colored regions) on Carl's billboard

	windowsHelper helper;

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

	triangleRelation getTriangleRelationship(const doubleTriangle &b, const doubleTriangle &f)
	{
		if (b == f) return DISCARD;

		triangleQuery v = getSharedVertices(b, f);	

		// Carl finds either 2, 1 or 0 shared vertices
		assert(v.hits >= 0 && v.hits <= 2);

		// Carl finds some intersection points...
		triangleIntersections intersections;
		doublePoint intersectionPoint;
		for (unsigned short i = 0; i < 3; i++) {
			for (unsigned short j = 0; j < 3; j++) {
				intersectionPoint = intersection(b.edge(i), f.edge(j));
				if (intersectionPoint != MIN_POINT)
				{
					intersections.pts[intersections.hits++] = triangleIntersectionPoint(i, j, intersectionPoint);
				}
			}
		}

		// Carl notices that the edges of b and f can intersect at either 2 or 4 points...
		assert(intersections.hits < 6 && (intersections.hits % 2 == 0));

		// Carl wants to know more about whether any of b's vertices are bounded by f 
		triangleQuery boundedVertices = boundVertices(b, f);

		switch (v.hits)
		{
			case 2:
			{
				// Carl found 2 co-linear triangles
				unsigned short sourceIndex, targetIndex;
				unsigned short nextSourceIndex, nextTargetIndex;

				doubleLine sourceEdge, targetEdge;
				doubleLine nextSourceEdge, nextTargetEdge;

				double cs, ct;

				triangleQuery e = getSharedEdges(b, f);
				// Carl can only find 1 shared edge
				assert(e.hits == 1);

				// For b and f, Carl finds the cross product of each triangle's 
				// shared edge with its corresponding next edge in each triangle.
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

				//	If the edge is going in different directions for b and f, 
				//	Carl flips f's cross product's sign
				if (sourceEdge != targetEdge) ct *= -1.0;

				if (cs * ct > 0.0) {

					int oppositeSourceIndex = (nextSourceIndex + 1) % 3;
					int oppositeTargetIndex = (nextTargetIndex + 1) % 3;

					doubleLine oppositeSourceEdge = b.edge(oppositeSourceIndex);
					doubleLine oppositeTargetEdge = f.edge(oppositeTargetIndex);

					doublePoint pt1 = intersection(nextSourceEdge, nextTargetEdge);
					doublePoint pt2 = intersection(nextSourceEdge, oppositeTargetIndex);
					doublePoint pt3 = intersection(oppositeSourceIndex, nextTargetEdge);
					doublePoint pt4 = intersection(nextSourceEdge, oppositeTargetEdge);

					//	Carl finds the intersection point between the triangles.
					//	if the point is a vertex belonging to b and lies on the edge of f, then Carl will discard b
					//	otherwise, Carl breaks b into 2 triangles, 
					//	with the intersection point acting as a shared vertex between the 2.
					//	Carl puts both triangles back into his bag, and discards whatever remains 

					return INTERSECTION;
				}
				else {
					return MISS;
				}
			}
			break;

			case 1:
			{
				//	Carl notices that if b and f share only 1 vertex, 
				//	they are either partially co-linear, in which case b and f will intersect at another point
				//	and one triangle will completely overlap with other,
				//	or they are just touching at the vertex, and can be handled as if they completely missed one another
				int lIndex = v.first;
				int rIndex = v.indices[lIndex];
				doubleLine lEdge = b.edge(lIndex);
				doubleLine rEdge = f.edge(rIndex);
				//doublePoint pt = intersection(lEdge, rEdge);

				//if (boundedVertices.hits == 0 && pt == doublePoint(MIN_DOUBLE, MIN_DOUBLE)) {
				//	return MISS;
				//}
			}
			break;

			case 0:
			{
				// Although no edges or vertices match, the triangles may still overlap in various ways...

				switch (intersections.hits) 
				{
				case 0: return MISS;
				case 2:
					{
/*
						b intersects f at 2 points
						there are 3 cases:
*//*
						1 edge in b intersects with 2 different edges in f
						 -	in which case, Carl creates 3 new smaller triangles, puts them into his bag, and discards the overlap
*//*
						2 edges in b intersect with 2 different edges in f
						 -	in which case, Carl creates 4 new smaller triangles, puts those into his bag, and discards the overlap
*/
					
					//	2 edges in b intersect with the same edge in f
					if (intersections.pts[0].sourceIndex != intersections.pts[1].sourceIndex && 
						intersections.pts[0].targetIndex == intersections.pts[1].targetIndex) {
					
						// 2 cases: either two vertices in b are bounded by f, or only 1 vertex in b is bounded

						assert(0 < boundedVertices.hits && boundedVertices.hits < 3);

						if (boundedVertices.hits == 1) {

							doublePoint v1 = b.vertex((boundedVertices.first + 1) % 3);
							doublePoint v2 = b.vertex((boundedVertices.first + 2) % 3);

							doubleTriangle t1(intersections.pts[0].point, intersections.pts[1].point, v1);
							doubleTriangle t2(intersections.pts[1].point, v1, v2);


							//	1 vertex in b is bounded by f
							//	Carl creates 2 new, smaller triangles from the intersection points and the remaining 2 vertices in b


							//	if results have two different sourceIndices, their sum is either [0] + [1] = 1, [0] + [2] = 2, or [1] + [2] = 3

							insertTriangle(bag, t1);
							insertTriangle(bag, t2);
							return DISCARD;
						} else {


							//	In which case, the bulk of the triangle (2 out of 3 vertices) is discarded
							//	Carl creates a new, smaller triangle from the intersection points and the remaining vertex in b

							//	if results have two different sourceIndices, their sum is either [0] + [1] = 1, [0] + [2] = 2, or [1] + [2] = 3

//							insertTriangle(bag, t);
						}
					
					} else if (intersections.pts[0].sourceIndex != intersections.pts[1].sourceIndex &&
						intersections.pts[0].targetIndex != intersections.pts[1].targetIndex) {

						doubleLine secondFEdge = f.edge(intersections.pts[1].targetIndex);
						doublePoint fVertex = secondFEdge.p;

						if (boundedVertices.hits == 1) {

							doublePoint v1 = b.vertex((boundedVertices.first + 1) % 3);
							doublePoint v2 = b.vertex((boundedVertices.first + 2) % 3);

							doubleTriangle t1(intersections.pts[0].point, v1, fVertex);
							doubleTriangle t2(intersections.pts[0].point, v1, v2);
							doubleTriangle t3(fVertex, v2, intersections.pts[1].point);

							insertTriangle(bag, t1);
							insertTriangle(bag, t2);
							insertTriangle(bag, t3);
							return DISCARD;
						}

					}
				}
				case 4:
					{
						doubleLine x;
						//doubleLine y;
						//doublePoint a, b, c;
						//sharedItems i;
					}
					return INTERSECTION;
				default:
					return MISS;
				}
			}
			break;

			default:
			{
				return MISS;
			}
			break;
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
	//// Carl also checks to see if the triangle edges intersect anywhere
	//		}
	//		if (v.sourceEdge == AB || v.sourceEdge == BC || v.sourceEdge == CA) {
	}

	void fillFragments()
	{
		//	Carl has already put the triangles making up all the capsules into his "bag", 
		//	Carl's bag is sorted by area, with the largest first.
		//	Carl also has a new, empty collection of triangles, the "fragments", 
		//	which are triangles already placed on the billboard.
		//	The bag can have many duplicates of what is in fragments. 
		//	Triangles in fragments do not share any area, 
		//	but they may share vertices or have co-linear, or partially co-linear sides

		//	while Carl finds a triangle b in his bag
		list<doubleTriangle>::iterator current = bag.begin(), next;
		doubleTriangle f;
		triangleRelation state = MISS;
		while (current != bag.end()) { // while !bag.empty()

			next = std::next(current, 1);
			//	while Carl finds a triangle f in fragments
			int i = 0, s = fragments.size();
			bool breakout = false;
			while (!breakout && i < s) {

				f = fragments[i++];
				state = getTriangleRelationship(*current, f);

				switch (state) {

				case DISCARD:
					//  Carl checks to see if b is completely enclosed by f
					//  	if b is completely surrounded by f, Carl throws b away
					//  	Carl breaks out and gets another b from the bag
					breakout = true;
					break;

				case B_SURROUNDS_F:
					//  Carl checks to see if f is completely enclosed by b
					//	(this seems impossible when you think about how Carl's bag is sorted by area, largest first...)
					//  	Carl supposes that if this scenario occurs, he could do the following: 
					//		if f is completely surrounded by b, then Carl breaks up b in the following way:
					//  	create 3 vertices at b's midpoints and along with b and f's vertices, break up b into 10 triangles
					//  	Carl puts the 9 triangles back into his bag and throws out the left-over copy of f
					//  	Carl breaks out and gets another b from the bag
					breakout = true;
					break;

				case MISS:
					//  Carl checks to see if f and b completely miss one another
					//  	if f and b completely miss each other, Carl gets another f from fragments
					break;

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
					breakout = true;
					break;
				};
			}

			//	Carl finishes checking every f in fragments and he is still holding b,
			//	So this is it ... Carl adds b to fragments 
			//		(where it magically transforms into an 'f' forever-more) 
			//		and gets a new b from his bag
			if (state == MISS) {
				fragments.push_back(*current);
			}
			//bag.remove(*current);
			current = next;
		}

		//if there are no triangles left in the bag, continue to circles...
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
				rand() % MAX_RADIUS + 10
			);
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
	
		helper.init(capsules, bag, fragments, min, d);
	}

	void resize(double d)
	{
		helper.resize(d);
	}

	void paint(Graphics &g)
	{
		helper.paint(g);
	}
};

