#pragma once
class IDrawer
{
public:
	IDrawer() {}
	virtual ~IDrawer() {}
	// Renders the object
	virtual int Draw(HDC hDC) = 0;
};

