#pragma once

#include "Math/Vector2.h"

#include <vector>

using namespace JD;

struct Direction
{
	Vector2<int> dir;
	float cst = 0.0f;
};

struct Node
{
	float fCost = 0.0f;
	float gCost = 0.0f;
	Vector2<int> pos;
	Vector2<int> prvPos;
};

struct Compare
{
	bool operator()(const Node& a, const Node& b) const
	{
		return a.fCost > b.fCost; // min heap
	}
};

class AStar
{
public:
	AStar();

public:
	std::vector<Vector2<int>> FindPath(const Vector2<int>& startPos, const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& costGrid);

private:
	std::vector<Vector2<int>> ConstructPath();
	float CalculateHeuristic(const Vector2<int> curPos);

private:
	Vector2<int> gridSize;
	Vector2<int> goalPos;
	std::vector<std::vector<Node>> nodes;
};
