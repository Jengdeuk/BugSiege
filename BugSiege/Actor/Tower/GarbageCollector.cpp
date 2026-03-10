#include "GarbageCollector.h"

#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Actor/Enemy/Enemy.h"

void GarbageCollector::TickAttack(float deltaTime)
{
	attackTimer.Tick(deltaTime);
	if (!attackTimer.IsTimeOut())
	{
		return;
	}

	targets = GetOwner()->As<GameLevel>()->QueryActorsInUniformGrid(Vector2<int>(GetPosition()), GetTowerData().radius);
	
	attackTimer.Reset();
	Attack();
}

void GarbageCollector::Attack()
{
	bool hasAttack = false;
	for (Actor* actor : targets)
	{
		Enemy* enemy = actor->As<Enemy>();
		if (!enemy->HasOccured() || enemy->HasFixed())
		{
			continue;
		}

		hasAttack = true;
		enemy->Damaged(GetTowerData().damage);
	}

	if (hasAttack)
	{
		PlayBackColorAnimation(GetTowerData().attackAnimSeq);

		GameLevel* level = GetOwner()->As<GameLevel>();
		static const Vector2<int> gridSize = Engine::Instance().GetGridSize();

		const Vector2<float>& pos = GetPosition();
		const int cx = static_cast<int>(std::round(pos.x));
		const int cy = static_cast<int>(std::round(pos.y));
		const float r = GetTowerData().radius;
		for (int y = static_cast<int>(std::round(pos.y - r)); y <= static_cast<int>(std::round(pos.y + r)); ++y)
		{
			for (int x = static_cast<int>(std::round(pos.x - r)); x <= static_cast<int>(std::round(pos.x + r)); ++x)
			{
				if (x == cx && y == cy)
				{
					continue;
				}

				if (x < 0 || x >= gridSize.x || y < 0 || y >= gridSize.y)
				{
					continue;
				}

				float dx = x - pos.x;
				float dy = y - pos.y;
				if (dx * dx + dy * dy <= r * r)
				{
					level->SummonExplosionEffect(Vector2<float>(static_cast<float>(x), static_cast<float>(y)));
				}
			}
		}
	}
}

void GarbageCollector::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::GarbageCollector, Vector2<int>(GetPosition()), -1);
}
