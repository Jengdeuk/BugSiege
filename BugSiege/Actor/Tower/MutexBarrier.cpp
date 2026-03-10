#include "MutexBarrier.h"

#include "Level/GameLevel.h"
#include "Actor/Enemy/Enemy.h"

void MutexBarrier::TickAttack(float deltaTime)
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

void MutexBarrier::Attack()
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
		enemy->Stunned();
	}
	
	if (hasAttack)
	{
		PlayBackColorAnimation(GetTowerData().attackAnimSeq);
	}
}

void MutexBarrier::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::MutexBarrier, Vector2<int>(GetPosition()), -1);
}
