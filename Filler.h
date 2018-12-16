#pragma once
#include "Canvas.h"
#include "IEventHandler.h"

class Filler :
	public IEventHandler
{
	HWND _hwnd;
	Canvas*	_canvas;
	ButtonChange::Id _state;
	COLORREF _color;

public:
	explicit Filler(HWND hwnd, Canvas* canvas);
	virtual ~Filler();
	void SetColor(COLORREF color);
	virtual int Handle(const Event::Id& e, const EventContext& c);
};

