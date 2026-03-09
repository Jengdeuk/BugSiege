#include "ExceptionHandler.h"

#include "Level/GameLevel.h"

void ExceptionHandler::UpdateGridForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::ExceptionHandler, Vector2<int>(GetPosition()), -1);
}
