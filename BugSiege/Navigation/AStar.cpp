#include "AStar.h"
#include "Engine/Engine.h"

#include <queue>
#include <cfloat>

static const float inf = FLT_MAX;
static const float diagonalCst = 1.41421356f;
static const std::vector<Direction> directions =
{
	{ { 0, 1 }, 1.0f }, { { 0, -1 }, 1.0f },
	{ { 1, 0 }, 1.0f }, { { -1, 0 }, 1.0f },
	{ { 1, 1 }, diagonalCst }, { { 1, -1 }, diagonalCst },
	{ { -1, 1 }, diagonalCst}, { { -1, -1 }, diagonalCst}
};

AStar::AStar()
{
	gridSize = Engine::Instance().GetGridSize();
	nodes.assign(gridSize.y, std::vector<Node>(gridSize.x, { inf, inf, { 0, 0 }, { -1, -1 } }));
}

std::vector<Vector2<int>> AStar::FindPath(const Vector2<int>& startPos, const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& costGrid)
{
	this->goalPos = goalPos;

	for (auto& row : nodes)
	{
		for (auto& node : row)
		{
			node.fCost = inf;
			node.gCost = inf;
			node.pos = { 0, 0 };
			node.prvPos = { -1,-1 };
		}
	}

	std::priority_queue<Node, std::vector<Node>, Compare> q;
	nodes[startPos.y][startPos.x] = { 0.0f, 0.0f, startPos, { -1, -1 } };
	q.emplace(nodes[startPos.y][startPos.x]);
	while (!q.empty())
	{
		float gCost = q.top().gCost;
		int y = q.top().pos.y;
		int x = q.top().pos.x;
		q.pop();

		if (gCost > nodes[y][x].gCost)
		{
			continue;
		}

		if (y == goalPos.y && x == goalPos.x)
		{
			return ConstructPath();
		}

		for (const Direction& direction : directions)
		{
			int ny = y + direction.dir.y;
			int nx = x + direction.dir.x;
			if (ny < 0 || ny >= gridSize.y || nx < 0 || nx >= gridSize.x)
			{
				continue;
			}
			
			if (!(ny == goalPos.y && nx == goalPos.x) && wallGrid[ny][nx])
			{
				continue;
			}

			// 대각선 뚫림 방지 처리
			if (wallGrid[ny][x] && wallGrid[y][nx])
			{
				continue;
			}

			const float ngCost = gCost + direction.cst + costGrid[ny][nx] * 5.0f;
			if (ngCost < nodes[ny][nx].gCost)
			{
				const float nfCost = ngCost + CalculateHeuristic({ nx, ny });
				nodes[ny][nx] = { nfCost, ngCost, { nx, ny }, { x, y } };
				q.emplace(nodes[ny][nx]);
			}
		}
	}

	return std::vector<Vector2<int>>();
}

std::vector<Vector2<int>> AStar::ConstructPath()
{
	std::vector<Vector2<int>> path;

	Vector2<int> curPos = goalPos;
	Vector2<int> end(-1, -1);
	while (nodes[curPos.y][curPos.x].prvPos != end)
	{
		path.emplace_back(curPos);
		curPos = nodes[curPos.y][curPos.x].prvPos;
	}

	return path;
}

float AStar::CalculateHeuristic(const Vector2<int> curPos)
{
	Vector2<int> dist = goalPos - curPos;
	return std::sqrtf(static_cast<float>(dist.x * dist.x + dist.y * dist.y));
}
