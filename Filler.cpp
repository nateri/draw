#include "stdafx.h"
#include "Resource.h"
#include "Filler.h"


Filler::Filler(HWND hwnd, Canvas* canvas) :
	_hwnd(hwnd),
	_canvas(canvas),
	_state(ButtonChange::Up),
	_color(RGB(0, 255, 128))
{
}

Filler::~Filler()
{
}

void Filler::SetColor(COLORREF color)
{
	_color = color;
}

int Filler::Handle(const Event::Id& e, const EventContext& c)
{
	switch (e) {
	case Event::MouseButton: {
		switch (c.mouseButton.id) {
		case MouseButtonId::Left:
			if (ButtonChange::None == c.mouseButton.state) {
				break;
			}
			if (!_canvas->IsInside(c.mouseButton.x, c.mouseButton.y)) {
				_state = ButtonChange::Up;
				return 0;
			}
			auto old_state = _state;
			_state = c.mouseButton.state;
			if (ButtonChange::Up == c.mouseButton.state &&
				ButtonChange::Down == old_state) {
				_canvas->Fill(c.mouseButton.x, c.mouseButton.y, _color);
			}
			return 1;
		}
	}
	return 0;
	default: return 0;
	}
}
