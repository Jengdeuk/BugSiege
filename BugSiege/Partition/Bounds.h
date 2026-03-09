#pragma once

#include "Math/Vector2.h"

using namespace JD;

struct Bounds
{
	int x = 0;
	int y = 0;
	int w = 1;
	int h = 1;

	inline int MaxX() const { return x + w; }
	inline int MaxY() const { return y + h; }
	inline bool IsIn(Vector2<int> pos) const { return pos.x >= x && pos.x < MaxX() && pos.y >= y && pos.y < MaxY(); }
};
