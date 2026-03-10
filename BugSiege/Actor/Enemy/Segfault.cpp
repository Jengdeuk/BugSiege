#include "Segfault.h"

#include "Render/Renderer.h"
#include "Navigation/AStar.h"

#include "Level/GameLevel.h"
#include "Partition/QuadNode.h"
#include "Actor/Tower/SystemCore.h"

static const int radius = 15;
static const int l = 2 * radius;

Segfault::Segfault()
	: navigation(std::make_unique<AStar>())
{
}

void Segfault::Draw()
{
	Super::Draw();

	for (const Vector2<int> pos : path)
	{
		Vector2<int> screenPos;
		if (!TransformWorldToScreen(Vector2<float>(pos), screenPos))
		{
			continue;
		}
		Renderer::Instance().Submit(".", screenPos, WORD(Color::Red), 5);
	}
}

void Segfault::SearchTarget()
{
	const int x = static_cast<int>(round(GetPosition().x));
	const int y = static_cast<int>(round(GetPosition().y));
	std::vector<Actor*> actors = GetOwner()->As<GameLevel>()->QueryActorsInQuadTree(Bounds{ x - radius, y - radius, l, l });

	target = nullptr;
	int tTier = 0;
	float tDist = 12345678.9f;
	for (Actor* const actor : actors)
	{
		Tower* tower = actor->As<Tower>();
		if (!tower->HasBuilt() || tower->HasCollapsed())
		{
			continue;
		}

		int tier = tower->GetTowerData().tier;
		float dist = LengthSq(actor->GetPosition() - GetPosition());
		if ((tier > tTier) || (tier == tTier && dist < tDist))
		{
			target = tower;
			tTier = tier;
			tDist = dist;
		}
	}
}

void Segfault::UpdatePath(const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& dangerGrid)
{
	path = navigation->FindPath(Vector2<int>(GetPosition()), goalPos, wallGrid, dangerGrid);
}

void Segfault::TickSearch(float deltaTime)
{
	SearchTarget();
	if (!IsTargetValid())
	{
		const Vector2<float> pos = GetPosition();
		float nearDist = 12345678.9f;
		for (auto& systemCore : GetOwner()->As<GameLevel>()->GetSystemCores())
		{
			float dist = sqrt(LengthSq(systemCore->GetPosition() - pos));
			if (dist < nearDist)
			{
				nearDist = dist;
				target = systemCore;
			}
		}

		//Vector2<float> nxtDst{ GetOwner()->As<GameLevel>()->GetNextNodeByFlowField(Vector2<int>(pos)) };
		const Vector2<float> dir{ (target->GetPosition() - pos).Normalized() };
		SetPosition(pos + dir * GetEnemyData().speed * deltaTime);

		return;
	}

	GameLevel* level = GetOwner()->As<GameLevel>();
	path = navigation->FindPath(Vector2<int>(GetPosition()), Vector2<int>(target->GetPosition()), level->GetWallGrid(), level->GetDangerGrid());
	ChangeState(State::Trace);
}

void Segfault::TickTrace(float deltaTime)
{
	// search target
	if (!IsTargetValid())
	{
		path.clear();
		ChangeState(State::Search);
		return;
	}

	// attack target
	if (CanAttack())
	{
		path.clear();
		ChangeState(State::Attack);
		return;
	}
	
	// search target
	if (HasArrived())
	{
		path.clear();
		ChangeState(State::Search);
		return;
	}

	const Vector2<float> pos{ GetPosition() };
	Vector2<float> nxtDst{ path.back() };
	if (round(pos.x) == nxtDst.x && round(pos.y) == nxtDst.y)
	{
		path.pop_back();
		if (path.size() <= 1)
		{
			return;
		}

		nxtDst = Vector2<float>(path.back());
	}

	const Vector2<float> dir{ (nxtDst - pos).Normalized() };
	SetPosition(pos + dir * GetEnemyData().speed * deltaTime);
}

void Segfault::TickAttack(float deltaTime)
{
	// search target
	if (!IsTargetValid())
	{
		ChangeState(State::Search);
		return;
	}

	// trace target
	if (!CanAttack())
	{
		ChangeState(State::Trace);
		return;
	}

	Super::TickAttack(deltaTime);
}

void Segfault::Damaged(const int damage)
{
	Super::Damaged(damage);

	if (HasFixed())
	{
		GetOwner()->As<GameLevel>()->SetSegfaultCount();
		path.clear();
	}
}

bool Segfault::IsTargetValid()
{
	return target && !target->DestroyRequested() && !target->HasCollapsed();
}

bool Segfault::HasArrived()
{
	return path.size() <= 1;
}

bool Segfault::CanAttack()
{
	return sqrt(LengthSq(target->GetPosition() - GetPosition())) <= GetEnemyData().radius;
}
