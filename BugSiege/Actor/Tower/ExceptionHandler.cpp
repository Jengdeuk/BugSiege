#include "ExceptionHandler.h"

#include "Level/GameLevel.h"

void ExceptionHandler::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::ExceptionHandler, Vector2<int>(GetPosition()), -1);
}
