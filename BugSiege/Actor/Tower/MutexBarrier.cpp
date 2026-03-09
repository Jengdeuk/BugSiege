#include "MutexBarrier.h"

#include "Level/GameLevel.h"

void MutexBarrier::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::MutexBarrier, Vector2<int>(GetPosition()), -1);
}
