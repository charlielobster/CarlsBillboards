#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "doublePoint.h"
#include "doubleLine.h"
#include "doubleTriangle.h"
#include "doubleRect.h"
#include "capsule.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class windowsHelper
{
public:
	int n;
	list<capsule>unitCapsules;

	list<doubleTriangle> unitBag;
	vector<doubleTriangle> unitFragments;

	vector<doubleLine> screenLines;
	vector<doubleRect> screenRects;

	list<doubleTriangle> screenBag;
	vector<doubleTriangle> screenFragments;

	void init(list<capsule> &capsules, list<doubleTriangle> &bag, vector<doubleTriangle> &fragments, double min, double d)
	{
		n = capsules.size();
		list<capsule>::iterator cptr = capsules.begin();
		while (cptr != capsules.end()) {
			insertCapsule(unitCapsules, (*cptr - min) / d);
			++cptr;
		}

		list<doubleTriangle>::iterator b = bag.begin();
		while (b != bag.end()) {
			unitBag.push_back((*b - min) / d);
			++b;
		}

		vector<doubleTriangle>::iterator f = fragments.begin();
		while (f != fragments.end()) {
			unitFragments.push_back((*f - min) / d);
			++f;
		}
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

		list<doubleTriangle>::iterator ub = unitBag.begin();
		while (ub != unitBag.end()) {
			screenBag.push_back(*ub * minScreen);
			++ub;
		}

		vector<doubleTriangle>::iterator uf = unitFragments.begin();
		while (uf != unitFragments.end()) {
			screenFragments.push_back(*uf * minScreen);
			++uf;
		}
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

	void paintFragments(Graphics &g)
	{
		Pen pen(Color(255, 0, 255));
		vector<doubleTriangle>::iterator sf = screenFragments.begin();
		while (sf != screenFragments.end()) {
			GraphicsPath path;
			Point a(sf->a.x, sf->a.y);
			Point b(sf->b.x, sf->b.y);
			Point c(sf->c.x, sf->c.y);
			path.AddLine(a, b);
			path.AddLine(b, c);
			path.AddLine(c, a);
			g.DrawPath(&pen, &path);
			++sf;
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
		//fillRects(g);
		//paintBag(g);
		paintFragments(g);
		//drawRects(g);
		paintRectEndPoints(g);
		//paintLines(g);
		//paintLineEndPoints(g);
	}
};