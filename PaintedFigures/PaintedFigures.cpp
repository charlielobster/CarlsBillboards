#include "stdafx.h"
#include "PaintedFigures.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <vector>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

using namespace std;

#define MAX_DOUBLE 1.79769e+308 
#define MIN_DOUBLE (-1.0 * MAX_DOUBLE)

#define MAX_RADIUS 49
#define MAX_DIMENSION 999
#define MAX_N 49

struct doublePoint {
public:
	doublePoint() : x(0.0), y(0.0) {}
	doublePoint(double x, double y) : x(x), y(y) {}
	friend doublePoint operator-(const doublePoint &p, const doublePoint &q) { return doublePoint(p.x - q.x, p.y - q.y); }
	double x = 0.0;
	double y = 0.0;
};

struct doubleTriangle
{
public:
	doublePoint vertices[3];
};

struct intTriangle
{
	Point a;
	Point b;
	Point c;
};

class Carl
{
public:
	double p0s[200][2];
	double p1s[200][2];
	double rs[200];
	double l0s[200][2];
	double l1s[200][2];
	double r0s[200][2];
	double r1s[200][2];
	double r2s[200][2];
	double r3s[200][2];
	double screen0s[200][2];
	double screen1s[200][2];
	double screenr0s[200][2];
	double screenr1s[200][2];
	double screenr2s[200][2];
	double screenr3s[200][2];
	doubleTriangle triangles[400];
	doubleTriangle screenTriangles[400];
	vector<doubleTriangle> tiles;
	vector<doubleTriangle> screenTiles;
	int tileCount = 0;

	unsigned short n;
	double min0 = MAX_DOUBLE, min1 = MAX_DOUBLE, minr00 = MAX_DOUBLE, minr01 = MAX_DOUBLE, minr10 = MAX_DOUBLE, minr11 = MAX_DOUBLE, minr20 = MAX_DOUBLE, minr21 = MAX_DOUBLE, minr30 = MAX_DOUBLE, minr31 = MAX_DOUBLE;
	double max0 = MIN_DOUBLE, max1 = MIN_DOUBLE, maxr00 = MIN_DOUBLE, maxr01 = MIN_DOUBLE, maxr10 = MIN_DOUBLE, maxr11 = MIN_DOUBLE, maxr20 = MIN_DOUBLE, maxr21 = MIN_DOUBLE, maxr30 = MIN_DOUBLE, maxr31 = MIN_DOUBLE;
	double min, max, d;

	double crossProduct(const doublePoint &p, const doublePoint &q)
	{
		return (q.y * p.x - q.x * p.y);
	}

	bool sameSide(doublePoint &p1, doublePoint &p2, const doublePoint &a, doublePoint &b)
	{
		double cp1 = crossProduct(b - a, p1 - a);
		double cp2 = crossProduct(b - a, p2 - a);
		if ((cp1 * cp2) >= 0.0) {
			return true;
		} else {
			return false;
		}
	}

	bool pointInTriangle(doublePoint &p, doubleTriangle &t) 
	{
		if (sameSide(p, t.vertices[0], t.vertices[1], t.vertices[2]) && 
			sameSide(p, t.vertices[1], t.vertices[0], t.vertices[2]) && 
			sameSide(p, t.vertices[2], t.vertices[0], t.vertices[1])) {
			return true;
		} else {
			return false;
		}
	}

	bool trianglesIntersect(int i, int j) 
	{
		// determine if co-planar triangles triangles[i] intersects (triangle) tiles[j]
		bool result = false;
		unsigned short k = 0;
		while (!result && k < 3) {
 			doublePoint p = triangles[i].vertices[k];
			result = pointInTriangle(p, tiles[j]);
			k++;
		}
		return result;
	}

	void init()
	{
		//srand((unsigned int)time(0));
		n = 20; // (rand() % MAX_N + 1);
		n += (n % 2);

		for (int i = 0; i < n; i++) {
			p0s[i][0] = rand() % MAX_DIMENSION + 1;
			p0s[i][1] = rand() % MAX_DIMENSION + 1;
			p1s[i][0] = rand() % MAX_DIMENSION + 1;
			p1s[i][1] = rand() % MAX_DIMENSION + 1;
			rs[i] = rand() % MAX_RADIUS + 1;

			double dx = p0s[i][1] - p1s[i][1];
			double dy = p1s[i][0] - p0s[i][0];
			double abs = sqrt(dx * dx + dy * dy);

			r0s[i][0] = p0s[i][0] - rs[i] * dx / abs;
			r0s[i][1] = p0s[i][1] - rs[i] * dy / abs;
			r1s[i][0] = p0s[i][0] + rs[i] * dx / abs;
			r1s[i][1] = p0s[i][1] + rs[i] * dy / abs;
			r2s[i][0] = p1s[i][0] - rs[i] * dx / abs;
			r2s[i][1] = p1s[i][1] - rs[i] * dy / abs;
			r3s[i][0] = p1s[i][0] + rs[i] * dx / abs;
			r3s[i][1] = p1s[i][1] + rs[i] * dy / abs;
		}

		for (int i = 0; i < n; i++) {

			if (p0s[i][0] < min0) { min0 = p0s[i][0]; }
			if (p0s[i][1] < min1) { min1 = p0s[i][1]; }
			if (p1s[i][0] < min0) { min0 = p1s[i][0]; }
			if (p1s[i][1] < min1) { min1 = p1s[i][1]; }
			if (p0s[i][0] > max0) { max0 = p0s[i][0]; }
			if (p0s[i][1] > max1) { max1 = p0s[i][1]; }
			if (p1s[i][0] > max0) { max0 = p1s[i][0]; }
			if (p1s[i][1] > max1) { max1 = p1s[i][1]; }

			if (r0s[i][0] < minr00) { minr00 = r0s[i][0]; }
			if (r0s[i][1] < minr01) { minr01 = r0s[i][1]; }
			if (r1s[i][0] < minr10) { minr10 = r1s[i][0]; }
			if (r1s[i][1] < minr11) { minr11 = r1s[i][1]; }
			if (r0s[i][0] > maxr00) { maxr00 = r0s[i][0]; }
			if (r0s[i][1] > maxr01) { maxr01 = r0s[i][1]; }
			if (r1s[i][0] > maxr10) { maxr10 = p1s[i][0]; }
			if (r1s[i][1] > maxr11) { maxr11 = p1s[i][1]; }
			if (r2s[i][0] < minr20) { minr20 = r2s[i][0]; }
			if (r2s[i][1] < minr21) { minr21 = r2s[i][1]; }
			if (r3s[i][0] < minr30) { minr30 = r3s[i][0]; }
			if (r3s[i][1] < minr31) { minr31 = r3s[i][1]; }
			if (r2s[i][0] > maxr20) { maxr20 = r2s[i][0]; }
			if (r2s[i][1] > maxr21) { maxr21 = r2s[i][1]; }
			if (r3s[i][0] > maxr30) { maxr30 = p1s[i][0]; }
			if (r3s[i][1] > maxr31) { maxr31 = p1s[i][1]; }
		}

		min = min(min(min(min(min(min(min(min(min(min0, min1), minr00), minr01), minr10), minr11), minr20), minr21), minr30), minr31) - MAX_RADIUS;
		max = max(max(max(max(max(max(max(max(max(max0, max1), maxr00), maxr01), maxr10), maxr11), maxr20), maxr21), maxr30), maxr31) + MAX_RADIUS;
		d = max - min;

		for (int i = 0; i < n; i++) {

			l0s[i][0] = (p0s[i][0] - min) / d;
			l0s[i][1] = (p0s[i][1] - min) / d;
			l1s[i][0] = (p1s[i][0] - min) / d;
			l1s[i][1] = (p1s[i][1] - min) / d;

			r0s[i][0] = (r0s[i][0] - min) / d;
			r0s[i][1] = (r0s[i][1] - min) / d;
			r1s[i][0] = (r1s[i][0] - min) / d;
			r1s[i][1] = (r1s[i][1] - min) / d;
			r2s[i][0] = (r2s[i][0] - min) / d;
			r2s[i][1] = (r2s[i][1] - min) / d;
			r3s[i][0] = (r3s[i][0] - min) / d;
			r3s[i][1] = (r3s[i][1] - min) / d;

			triangles[i].vertices[0].x = r0s[i][0];
			triangles[i].vertices[0].y = r0s[i][1];
			triangles[i].vertices[1].x = r1s[i][0];
			triangles[i].vertices[1].y = r1s[i][1];
			triangles[i].vertices[2].x = r2s[i][0];
			triangles[i].vertices[2].y = r2s[i][1];

			triangles[n + i].vertices[0].x = r1s[i][0];
			triangles[n + i].vertices[0].y = r1s[i][1];
			triangles[n + i].vertices[1].x = r2s[i][0];
			triangles[n + i].vertices[1].y = r2s[i][1];
			triangles[n + i].vertices[2].x = r3s[i][0];
			triangles[n + i].vertices[2].y = r3s[i][1];

		}

		tiles.push_back(triangles[0]);
		tiles.push_back(triangles[n]);
		tileCount = 2;

		for (int i = 1; i < n; i++) {
			bool result0 = false;
			bool resultn = false;
			unsigned short j = 0;
			while (!result0 && !resultn && (j < tileCount)) {
				result0 = trianglesIntersect(i, j);
				resultn = trianglesIntersect(n + i, j);
				j++;
			}
			if (!result0) {
				tiles.push_back(triangles[i]);
				tileCount++;
			}
			if (!resultn) {
				tiles.push_back(triangles[n + i]);
				tileCount++;
			}
		}
		screenTiles.resize(tileCount);
	}

	void resize(double minScreen)
	{
		for (int i = 0; i < n; i++) {

			screen0s[i][0] = l0s[i][0] * minScreen;
			screen0s[i][1] = l0s[i][1] * minScreen;
			screen1s[i][0] = l1s[i][0] * minScreen;
			screen1s[i][1] = l1s[i][1] * minScreen;

			screenr0s[i][0] = r0s[i][0] * minScreen;
			screenr0s[i][1] = r0s[i][1] * minScreen;
			screenr1s[i][0] = r1s[i][0] * minScreen;
			screenr1s[i][1] = r1s[i][1] * minScreen;
			screenr2s[i][0] = r2s[i][0] * minScreen;
			screenr2s[i][1] = r2s[i][1] * minScreen;
			screenr3s[i][0] = r3s[i][0] * minScreen;
			screenr3s[i][1] = r3s[i][1] * minScreen;

			screenTriangles[i].vertices[0].x = triangles[i].vertices[0].x * minScreen;
			screenTriangles[i].vertices[0].y = triangles[i].vertices[0].y * minScreen;
			screenTriangles[i].vertices[1].x = triangles[i].vertices[1].x * minScreen;
			screenTriangles[i].vertices[1].y = triangles[i].vertices[1].y * minScreen;
			screenTriangles[i].vertices[2].x = triangles[i].vertices[2].x * minScreen;
			screenTriangles[i].vertices[2].y = triangles[i].vertices[2].y * minScreen;

			screenTriangles[n + i].vertices[0].x = triangles[n + i].vertices[0].x * minScreen;
			screenTriangles[n + i].vertices[0].y = triangles[n + i].vertices[0].y * minScreen;
			screenTriangles[n + i].vertices[1].x = triangles[n + i].vertices[1].x * minScreen;
			screenTriangles[n + i].vertices[1].y = triangles[n + i].vertices[1].y * minScreen;
			screenTriangles[n + i].vertices[2].x = triangles[n + i].vertices[2].x * minScreen;
			screenTriangles[n + i].vertices[2].y = triangles[n + i].vertices[2].y * minScreen;
		}

		for (int i = 0; i < tileCount; i++) {

			screenTiles[i].vertices[0].x = tiles[i].vertices[0].x * minScreen;
			screenTiles[i].vertices[0].y = tiles[i].vertices[0].y * minScreen;
			screenTiles[i].vertices[1].x = tiles[i].vertices[1].x * minScreen;
			screenTiles[i].vertices[1].y = tiles[i].vertices[1].y * minScreen;
			screenTiles[i].vertices[2].x = tiles[i].vertices[2].x * minScreen;
			screenTiles[i].vertices[2].y = tiles[i].vertices[2].y * minScreen;
		}
	}

	void fillRect(Graphics &g, int i)
	{
		Point r0(screenr0s[i][0], screenr0s[i][1]);
		Point r1(screenr1s[i][0], screenr1s[i][1]);
		Point r2(screenr2s[i][0], screenr2s[i][1]);
		Point r3(screenr3s[i][0], screenr3s[i][1]);

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
		Point r0(screenr0s[i][0], screenr0s[i][1]);
		Point r1(screenr1s[i][0], screenr1s[i][1]);
		Point r2(screenr2s[i][0], screenr2s[i][1]);
		Point r3(screenr3s[i][0], screenr3s[i][1]);

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
		Point a(screenTiles[i].vertices[0].x, screenTiles[i].vertices[0].y);
		Point b(screenTiles[i].vertices[1].x, screenTiles[i].vertices[1].y);
		Point c(screenTiles[i].vertices[2].x, screenTiles[i].vertices[2].y);
		path.AddLine(a, b);
		path.AddLine(b, c);
		path.AddLine(c, a);
		g.DrawPath(&p, &path);
	}

	void drawTriangles(Graphics &g, int i)
	{
		Pen p(Color(255, 0, 0));
		GraphicsPath path;
		Point a(screenTriangles[i].vertices[0].x, screenTriangles[i].vertices[0].y);
		Point b(screenTriangles[i].vertices[1].x, screenTriangles[i].vertices[1].y);
		Point c(screenTriangles[i].vertices[2].x, screenTriangles[i].vertices[2].y);
		path.AddLine(a, b);
		path.AddLine(b, c);
		path.AddLine(c, a);
		g.DrawPath(&p, &path);
	}

	void drawRectEndPoints(Graphics &g, int i)
	{
		Pen p(Color(255, 0, 0), 4);
		g.DrawEllipse(&p, screenr0s[i][0] - 2, screenr0s[i][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenr1s[i][0] - 2, screenr1s[i][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenr2s[i][0] - 2, screenr2s[i][1] - 2, 4, 4);
		g.DrawEllipse(&p, screenr3s[i][0] - 2, screenr3s[i][1] - 2, 4, 4);
	}

	void paintLine(Graphics &g, int i)
	{
		// Create pen.
		Pen redPen(Color(255, 255, 255), 1);
		Point p0(screen0s[i][0], screen0s[i][1]);
		Point p1(screen1s[i][0], screen1s[i][1]);
		g.DrawLine(&redPen, p0, p1);
	}

	void paintLineEndPoints(Graphics &g, int i)
	{
		Pen blackPen(Color(0, 0, 0), 6);
		g.DrawEllipse(&blackPen, screen0s[i][0] - 3, screen0s[i][1] - 3, 6, 6);
		g.DrawEllipse(&blackPen, screen1s[i][0] - 3, screen1s[i][1] - 3, 6, 6);
	}

	void paint(Graphics &g)
	{
		for (int i = 0; i < n; i++) {
			fillRect(g, i);
		}
		for (int i = 0; i < tileCount; i++) {
			drawTiles(g, i);
		}
		//for (int i = 0; i < n; i++) {
		//	drawTriangles(g, i);
		//	drawTriangles(g, n + i);
		//}
		//for (int i = 0; i < n; i++) {
		//	drawRect(g, i);
		//}
		//for (int i = 0; i < n; i++) {
		//	drawRectEndPoints(g, i);
		//}
		//for (int i = 0; i < n; i++) {
		//	paintLine(g, i);
		//}
		//for (int i = 0; i < n; i++) {
		//	paintLineEndPoints(g, i);
		//}
	}
};

Carl c;

double width, height, minScreen;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("GettingStarted");

	RegisterClass(&wndClass);

	c.init();

	hWnd = CreateWindow(
		TEXT("GettingStarted"),   // window class name
		TEXT("Getting Started"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message)
	{
	case WM_SIZE:
		{
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			minScreen = min(width, height);
			c.resize(minScreen);
		}
		return 0;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			Graphics g(hdc);
			c.paint(g);
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc