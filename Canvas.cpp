#include "stdafx.h"
#include <vector>
#include <cassert>
#include "Canvas.h"

using namespace std;

Canvas::Canvas(int width, int height, COLORREF color) :
	_mem(nullptr),
	_width(width),
	_height(height),
	_bmp(nullptr),
	_old(nullptr),
	_alloc(false)
{
	// Create starting palette
	_mem = (COLORREF*)malloc(sizeof(COLORREF) * _width * _height);
	if (!_mem) {
		return;
	}

	int len = _width * _height;
	for (int i = 0; i < len; ++i) {
		_mem[i] = color;
	}
}

Canvas::~Canvas()
{
	if (nullptr != _old) {
		SelectObject(_hdc, _old);
	}
	if (nullptr != _hdc) {
		DeleteDC(_hdc);
	}
	_hdc = nullptr;

	free(_mem);
	_mem = nullptr;
}

bool Canvas::Init(HWND hWnd, HDC hDC)
{
	_hwnd = hWnd;
	_hdc = CreateCompatibleDC(hDC);
	_bmp = CreateCompatibleBitmap(hDC, _width, _height);

	_old = SelectObject(_hdc, _bmp);
	// _old != _bmp
	assert(_old != _bmp);

	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			const int index = _width * y + x;
			SetPixel(_hdc, x, y, _mem[index]);
		}
	}

	return true;
}

struct Node {
	const int x;
	const int y;
};
typedef vector<Node> Branches;

class FillIter {
	Canvas* canvas;
	int x;
	int y;
	COLORREF color;
	Branches branches;

	bool evalRight(int& rX, int& rY) const
	{
		rX = x + 1;
		rY = y;
		if (!canvas->IsInside(rX, rY)) {
			return false;
		}
		return color == canvas->Color(rX, rY);
	}
	bool evalUp(int& rX, int& rY) const
	{
		rX = x;
		rY = y - 1;
		if (!canvas->IsInside(rX, rY)) {
			return false;
		}
		return color == canvas->Color(rX, rY);
	}
	bool evalLeft(int& rX, int& rY) const 
	{
		rX = x - 1;
		rY = y;
		if (!canvas->IsInside(rX, rY)) {
			return false;
		}
		return color == canvas->Color(rX, rY);
	}
	bool evalDown(int& rX, int& rY) const
	{
		rX = x;
		rY = y + 1;
		if (!canvas->IsInside(rX, rY)) {
			return false;
		}
		return color == canvas->Color(rX, rY);
	}

	void eval()
	{
		assert(canvas->IsInside(x, y));
		color = canvas->Color(x, y);

		int nextX, nextY;
		if (evalRight(nextX, nextY)) {
			branches.push_back(Node{ nextX, nextY });
		}
		if (evalUp(nextX, nextY)) {
			branches.push_back(Node{ nextX, nextY });
		}
		if (evalLeft(nextX, nextY)) {
			branches.push_back(Node{ nextX, nextY });
		}
		if (evalDown(nextX, nextY)) {
			branches.push_back(Node{ nextX, nextY });
		}
	}

public:
	FillIter(Canvas* canvas, int x, int y) :
		canvas(canvas), x(x), y(y)
	{
		branches.reserve(canvas->Count());
		eval();
	}

	bool IsValid() const {
		if (!canvas->IsInside(x, y)) {
			return false;
		}
		return true;
	}

	int X() const { return x; }

	int Y() const { return y; }

	void Next()
	{
		for (; branches.size(); branches.pop_back()) {
			auto iter = branches.at(branches.size() - 1);
			x = iter.x;
			y = iter.y;
			if (color == canvas->Color(x, y)) {
				eval();
				return;
			}
		}
		x = -1; y = -1;
	}
};

size_t Canvas::Count()
{
	return _width * _height;
}

bool Canvas::IsInside(int x, int y)
{
	return x >= 0 && y >= 0 && x < _width && y < _height;
}

COLORREF Canvas::Color(int x, int y)
{
	int index = y * _width + x;
	return _mem[index];
}

int Canvas::Clear(COLORREF color)
{
	// Update memory and HDC
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {
			const int index = y * _width + x;
			_mem[index] = color;
			SetPixel(_hdc, x, y, _mem[index]);
		}
	}
	InvalidateRect(_hwnd, nullptr, false);
	return _width * _height;
}

int Canvas::Fill(int x, int y, COLORREF color)
{
	int origin = y * _width + x;
	COLORREF oldColor = _mem[origin];
	if (color == oldColor) {
		return 0;
	}

	int count = 0;
	FillIter iter(this, x, y);
	for (; iter.IsValid(); iter.Next()) {
		++count;
		int index = iter.Y() * _width + iter.X();
		_mem[index] = color;
		SetPixel(_hdc, iter.X(), iter.Y(), color);
	}

	InvalidateRect(_hwnd, nullptr, false);
	return count;
}

int Canvas::Scratch()
{
	// Generate random lines
	int numSides = rand() % 45;
	vector<POINT> sides;
	for (int i = 0; i < numSides; ++i) {
		int x = rand() % _width;
		int y = rand() % _height;
		sides.push_back(POINT{ x, y });
	}
	// Draw the lines onto the draw context
	Polyline(_hdc, sides.data(), sides.size());
	// Update the bitmap with the lines
	int count = 0;
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {
			const int index = _width * y + x;
			const COLORREF color = GetPixel(_hdc, x, y);
			if (_mem[index] != color) {
				++count;
			}
			_mem[index] = color;
		}
	}
	return count;
}

int Canvas::Draw(HDC hDC)
{
	BitBlt(hDC, 0, 0, _width, _height, _hdc, 0, 0, SRCCOPY);
	return _width*_height;
}

