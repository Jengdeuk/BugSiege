#pragma once

#include "QuadNode.h"

class QuadTree
{
public:
	QuadTree();

public:
	void Insert(Actor* actor);
	std::vector<Actor*> Query(const Bounds& bounds);

public:
	void Draw();

private:
	Vector2<int> gridSize;
	std::unique_ptr<QuadNode> root;
};
