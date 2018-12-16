#pragma once
#include "IDrawer.h"

class Canvas :
	public IDrawer
{
	COLORREF* _mem;
	int _width;
	int _height;
	HWND _hwnd;
	HDC _hdc;
	HBITMAP _bmp;
	HGDIOBJ _old;
	bool _alloc;

public:
	explicit Canvas(int width, int height, COLORREF color);
	virtual ~Canvas();
	// Initializes the object context
	bool Init(HWND hWnd, HDC hDC);
	// Number of pixels in canvas
	size_t Count();
	// Renders the object
	virtual int Draw(HDC hDC);
	// Inspects if coord is within Canvas
	bool IsInside(int x, int y);
	// Inspects the color at coord
	COLORREF Color(int x, int y);
	// Resets the color of the entire canvas
	int Clear(COLORREF c);
	// Changes the color of all attached similar colors
	int Fill(int x, int y, COLORREF c);
	// Draws random lines on the canvas
	int Scratch();
};

