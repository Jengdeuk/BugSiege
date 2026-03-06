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
	void QueryRecursive(const Bounds& bounds, std::vector<QuadNode*>& possibleNodes);
	void DrawRecursive();

public:
	inline const Bounds& GetBounds() const { return bounds; }
	inline const std::vector<Actor*>& GetActors() const { return actors; }

private:
	bool IsDivided();
	bool Subdivide();
	std::vector<QuadType> GetQuadTypes(const Bounds& bounds);
	QuadType TestRegion(const Bounds& bounds);
	bool TransformWorldToScreen(const Vector2<int>& worldPos, Vector2<int>& outScreenPos);
	void DrawNumber(const Vector2<int>& worldPos);
	void DrawPoint(const Vector2<int>& worldPos);

private:
	int depth = 0;
	Bounds bounds;
	std::vector<Actor*> actors;

private:
	std::unique_ptr<QuadNode> topLeft;
	std::unique_ptr<QuadNode> topRight;
	std::unique_ptr<QuadNode> bottomLeft;
	std::unique_ptr<QuadNode> bottomRight;
};
