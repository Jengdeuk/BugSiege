#include "QuadTree.h"

#include "Engine/Engine.h"
#include "Actor/Actor.h"

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

void QuadTree::Remove(Actor* actor)
{
	if (!actor)
	{
		return;
	}

	QuadNode* node = actor->GetOwnerQuadNode();
	if (!node)
	{
		return;
	}

	if (node->Remove(actor))
	{
		node = node->GetParent();
		while (node)
		{
			if (node->CanMerge())
			{
				node->MergeChildren();
			}

			node = node->GetParent();
		}
	}
}

std::vector<Actor*> QuadTree::Query(const Bounds& bounds)
{
	std::vector<QuadNode*> possibleNodes;
	root->QueryRecursive(bounds, possibleNodes);

	std::vector<Actor*> intersects;
	for (QuadNode* const node : possibleNodes)
	{
		for (Actor* const actor : node->GetActors())
		{
			if (bounds.IsIn(Vector2<int>(actor->GetPosition())))
			{
				intersects.emplace_back(actor);
			}
		}
	}

	return intersects;
}

void QuadTree::Draw()
{
	//root->DrawRecursive();
}