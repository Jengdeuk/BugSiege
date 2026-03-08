#include "GarbageCollector.h"

#include "Level/GameLevel.h"

void GarbageCollector::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::GarbageCollector, Vector2<int>(GetPosition()), -1);
}
