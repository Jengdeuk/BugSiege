#pragma once

#include "Enemy.h"

#include <vector>

using namespace JD;

class AStar;
class Tower;

class Segfault : public Enemy
{
	RTTI_DECLARATIONS(Segfault, Enemy)

public:
	Segfault();

public:
	virtual void Draw() override;

public:
	void SearchTarget();
	void UpdatePath(const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& dangerGrid);

private:
	virtual void TickSearch(float deltaTime) override;
	virtual void TickTrace(float deltaTime) override;
	virtual void TickAttack(float deltaTime) override;

private:
	virtual void Attack() override;

private:
	bool IsTargetValid();
	bool HasArrived();
	bool CanAttack();

private:
	Tower* target = nullptr;
	std::vector<Vector2<int>> path;
	std::unique_ptr<AStar> navigation;
};
