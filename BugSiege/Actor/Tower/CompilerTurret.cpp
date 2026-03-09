#include "CompilerTurret.h"

#include "Level/GameLevel.h"

void CompilerTurret::UpdateGridForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::CompilerTurret, Vector2<int>(GetPosition()), -1);
}
