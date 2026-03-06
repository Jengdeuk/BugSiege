#include "QuadTree.h"

#include "Engine/Engine.h"

QuadTree::QuadTree()
{
	gridSize = Engine::Instance().GetGridSize();
	root = std::make_unique<QuadNode>(Bounds{ 0, 0, gridSize.x, gridSize.y });
}

void QuadTree::Insert(Actor* actor)
{
	if (!actor)
	{
		return;
	}

	root->InsertRecursive(actor);
}

std::vector<Actor*> QuadTree::Query(const Bounds& bounds)
{
	return std::vector<Actor*>();
}

void QuadTree::Draw()
{
	root->DrawRecursive();
}