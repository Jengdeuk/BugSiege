#include "DebuggerNode.h"

#include "Level/GameLevel.h"

void DebuggerNode::UpdateGridsForNavigation()
{
	GetOwner()->As<GameLevel>()->UpdateGridsForNavigation(TowerType::DebuggerNode, Vector2<int>(GetPosition()), -1);
}
