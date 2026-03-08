#include "CompilerTurret.h"

#include "Level/GameLevel.h"

void CompilerTurret::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::CompilerTurret, Vector2<int>(GetPosition()), -1);
}
