#include "DebuggerNode.h"

#include "Level/GameLevel.h"
#include "Actor/Enemy/Enemy.h"

void DebuggerNode::Attack()
{
	Actor* target = nullptr;
	int lHP = 12345;
	const Vector2<float>& pos = GetPosition();
	for (Actor* actor : targets)
	{
		Enemy* enemy = actor->As<Enemy>();
		if (!enemy->HasOccured() || enemy->HasFixed())
		{
			continue;
		}

		int heath = enemy->GetEnemyData().health;
		if (heath < lHP)
		{
			heath = lHP;
			target = actor;
		}
	}

	if (target && !target->DestroyRequested() && !target->As<Enemy>()->HasFixed())
	{
		PlayBackColorAnimation(GetTowerData().attackAnimSeq);
		target->As<Enemy>()->Damaged(GetTowerData().damage);
	}
}

void DebuggerNode::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::DebuggerNode, Vector2<int>(GetPosition()), -1);
}
