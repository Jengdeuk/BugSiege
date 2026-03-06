#pragma once

#include "Enemy.h"

#include <vector>
#include <memory>

using namespace JD;

class AStar;

class Segfault : public Enemy
{
	RTTI_DECLARATIONS(Segfault, Enemy)

public:
	Segfault();

public:
	virtual void Draw() override;

public:
	void UpdatePath(const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& dangerGrid);

private:
	std::vector<Vector2<int>> path;
	std::unique_ptr<AStar> navigation;
};
