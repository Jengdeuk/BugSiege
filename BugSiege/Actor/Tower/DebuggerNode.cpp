#include "DebuggerNode.h"

#include "Level/GameLevel.h"

void DebuggerNode::UpdateDangerGrid()
{
	GetOwner()->As<GameLevel>()->UpdateDangerGrid(TowerType::DebuggerNode, Vector2<int>(GetPosition()), -1);
}
