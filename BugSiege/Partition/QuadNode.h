#pragma once

#include "Math/Vector2.h"

#include <vector>
#include <memory>

namespace JD
{
	class Actor;
}

using namespace JD;

struct Bounds
{
	int x = 0;
	int y = 0;
	int w = 1;
	int h = 1;

	inline int MaxX() const { return x + w; }
	inline int MaxY() const { return y + h; }
	inline bool IsIn(Vector2<int> pos) const { return pos.x >= x && pos.x < MaxX() && pos.y >= y && pos.y < MaxY(); }
};

enum class QuadType
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	Straddling,
	OutOfArea
};

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
