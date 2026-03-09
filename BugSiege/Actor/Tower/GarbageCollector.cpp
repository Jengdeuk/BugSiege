#include "GarbageCollector.h"

#include "Level/GameLevel.h"

void GarbageCollector::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::GarbageCollector, Vector2<int>(GetPosition()), -1);
}
