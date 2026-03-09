#include "UniformGrid.h"

#include "Engine/Engine.h"
#include "Actor/Actor.h"

UniformGrid::UniformGrid(const int cellSize)
	: cellSize(cellSize)
{
	gridSize = Engine::Instance().GetGridSize();
	actorsGrid.assign(gridSize.y / cellSize, std::vector<std::vector<Actor*>>(gridSize.x / cellSize, std::vector<Actor*>()));
}

void UniformGrid::Insert(const Vector2<int>& cellPos, Actor* actor)
{
	actorsGrid[cellPos.y][cellPos.x].emplace_back(actor);
}

void UniformGrid::Remove(const Vector2<int>& cellPos, Actor* actor)
{
	if (cellPos == Vector2<int>(-1, -1))
	{
		return;
	}

	auto it = actorsGrid[cellPos.y][cellPos.x].begin();
	while (it != actorsGrid[cellPos.y][cellPos.x].end())
	{
		if (*it == actor)
		{
			actorsGrid[cellPos.y][cellPos.x].erase(it);
			break;
		}
		++it;
	}
}

std::vector<Actor*> UniformGrid::Query(const Vector2<int>& pos, const float r)
{
	std::vector<Actor*> intersects;

	const int cx = pos.x;
	const int cy = pos.y;
	const int sx = static_cast<int>(round(cx - r));
	const int ex = static_cast<int>(round(cx + r));
	const int sy = static_cast<int>(round(cy - r));
	const int ey = static_cast<int>(round(cy + r));
	for (int y = (sy) / cellSize; y <= (ey) / cellSize; ++y)
	{
		for (int x = (sx) / cellSize; x <= (ex) / cellSize; ++x)
		{
			if (x < 0 || x >= gridSize.x / cellSize || y < 0 || y >= gridSize.y / cellSize)
			{
				continue;
			}

			for (Actor* actor : actorsGrid[y][x])
			{
				const Vector2<float> apos{ actor->GetPosition() };
				float dx = apos.x - cx;
				float dy = apos.y - cy;
				if (dx * dx + dy * dy <= r * r)
				{
					intersects.emplace_back(actor);
				}
			}
		}
	}

	return intersects;
}
