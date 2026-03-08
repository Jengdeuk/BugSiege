#include "MutexBarrier.h"

#include "Level/GameLevel.h"

void MutexBarrier::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::MutexBarrier, Vector2<int>(GetPosition()), -1);
}
