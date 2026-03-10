#include "ExceptionHandler.h"

#include "Level/GameLevel.h"
#include "Actor/Enemy/Enemy.h"

void ExceptionHandler::Attack()
{
	Actor* target = nullptr;
	int ht = 0;
	for (Actor* actor : targets)
	{
		Enemy* enemy = actor->As<Enemy>();
		if (!enemy->HasOccured() || enemy->HasFixed())
		{
			continue;
		}

		const int t = enemy->GetEnemyData().tier;
		if (t > ht)
		{
			ht = t;
			target = actor;
		}
	}

	if (target && !target->DestroyRequested() && !target->As<Enemy>()->HasFixed())
	{
		PlayBackColorAnimation(GetTowerData().attackAnimSeq);
		target->As<Enemy>()->Damaged(GetTowerData().damage);
	}
}

void ExceptionHandler::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::ExceptionHandler, Vector2<int>(GetPosition()), -1);
}
