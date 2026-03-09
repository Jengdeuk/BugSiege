#pragma once

#include "Bounds.h"

#include <vector>
#include <memory>

enum class QuadType
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	Straddling,
	OutOfArea
};

namespace JD
{
	class Actor;
}

class QuadNode
{
public:
	QuadNode(const Bounds& bounds, int depth = 0);

public:
	void InsertRecursive(Actor* actor);
	bool Remove(Actor* actor);
	void QueryRecursive(const Bounds& bounds, std::vector<QuadNode*>& possibleNodes);
	void DrawRecursive();

public:
	bool CanMerge();
	void MergeChildren();

public:
	inline const Bounds& GetBounds() const { return bounds; }
	inline const std::vector<Actor*>& GetActors() const { return actors; }
	inline QuadNode* const GetParent() const { return parent; }

private:
	bool IsDivided();
	void Subdivide();
	std::vector<QuadType> GetQuadTypes(const Bounds& bounds);
	QuadType TestRegion(const Bounds& bounds);

private:
	bool TransformWorldToScreen(const Vector2<int>& worldPos, Vector2<int>& outScreenPos);
	void DrawNumber(const Vector2<int>& worldPos);
	void DrawPoint(const Vector2<int>& worldPos);

private:
	int depth = 0;
	Bounds bounds;
	std::vector<Actor*> actors;

private:
	QuadNode* parent = nullptr;
	std::unique_ptr<QuadNode> topLeft;
	std::unique_ptr<QuadNode> topRight;
	std::unique_ptr<QuadNode> bottomLeft;
	std::unique_ptr<QuadNode> bottomRight;
};
