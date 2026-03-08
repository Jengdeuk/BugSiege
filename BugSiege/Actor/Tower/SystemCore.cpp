#include "SystemCore.h"

#include "Level/GameLevel.h"

void SystemCore::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::SystemCore, Vector2<int>(GetPosition()), -1);
}
