#include "SystemCore.h"

#include "Level/GameLevel.h"

void SystemCore::Damaged(const int damage)
{
	PlayColorAnimation(GetTowerData().damagedAnimSeq);
	GetOwner()->As<GameLevel>()->DamagedSystemCore(damage);
}

void SystemCore::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::SystemCore, Vector2<int>(GetPosition()), -1);
}
