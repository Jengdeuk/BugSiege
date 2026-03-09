#include "DebuggerNode.h"

#include "Level/GameLevel.h"

void DebuggerNode::UpdateGridForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::DebuggerNode, Vector2<int>(GetPosition()), -1);
}
