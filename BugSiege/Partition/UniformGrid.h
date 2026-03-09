#pragma once

#include "Math/Vector2.h"

#include <vector>

namespace JD
{
	class Actor;
}

using namespace JD;

class UniformGrid
{
public:
	UniformGrid(const int cellSize);

public:
	void Insert(const Vector2<int>& cellPos, Actor* actor);
	void Remove(const Vector2<int>& cellPos, Actor* actor);
	std::vector<Actor*> Query(const Vector2<int>& pos, const float r);

private:
	int cellSize = 0;
	Vector2<int> gridSize;
	std::vector<std::vector<std::vector<Actor*>>> actorsGrid;
};

