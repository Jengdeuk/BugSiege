#include "CompilerTurret.h"

#include "Level/GameLevel.h"
#include "Actor/Enemy/Enemy.h"

void CompilerTurret::Attack()
{
	Actor* target = nullptr;
	float nDist = 12345678.9f;
	const Vector2<float>& pos = GetPosition();
	for (Actor* actor : targets)
	{
		Enemy* enemy = actor->As<Enemy>();
		if (!enemy->HasOccured() || enemy->HasFixed())
		{
			continue;
		}

		const float dist = LengthSq(actor->GetPosition() - pos);
		if (dist < nDist)
		{
			nDist = dist;
			target = actor;
		}
	}

	if (target && !target->DestroyRequested() && !target->As<Enemy>()->HasFixed())
	{
		PlayBackColorAnimation(GetTowerData().attackAnimSeq);
		target->As<Enemy>()->Damaged(GetTowerData().damage);
	}
}

void CompilerTurret::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::CompilerTurret, Vector2<int>(GetPosition()), -1);
}
