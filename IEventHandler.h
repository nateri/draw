#pragma once

struct Event {
	enum Id {
		None = 0,
		MouseButton,
		Key,
	};
};
struct MouseButtonId {
	enum Id {
		None = 0,
		Left,
		Middle,
		Right,
	};
};
struct ButtonChange {
	enum Id {
		None = 0,
		Up,
		Down,
	};
};

struct EventContext {
	union {
		struct MouseButton {
			MouseButtonId::Id id;
			ButtonChange::Id state;
			int x;
			int y;
			DWORD flags;
		} mouseButton;
	};
};
class IEventHandler
{
public:
	IEventHandler() {}
	virtual ~IEventHandler() {}
	// Process Event
	virtual int Handle(const Event::Id& e, const EventContext& c) = 0;
};

