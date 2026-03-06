#include "QuadNode.h"

#include "Render/Renderer.h"
#include "Actor/Actor.h"
#include "Engine/Engine.h"

static const int maxDepth = 5;
static const char* imgs[] = { "0", "1", "2", "3", "4", "5" };

QuadNode::QuadNode(const Bounds& bounds, int depth)
	: bounds(bounds), depth(depth)
{
}

void QuadNode::InsertRecursive(Actor* actor)
{
	const Vector2<int> actorPos = Vector2<int>(actor->GetPosition());
	QuadType type = TestRegion({ actorPos.x, actorPos.y, 1, 1 });
	if (type == QuadType::OutOfArea)
	{
		return;
	}

	if (Subdivide())
	{
		switch (type)
		{
		case QuadType::TopLeft:
			topLeft->InsertRecursive(actor);
			break;
		case QuadType::TopRight:
			topRight->InsertRecursive(actor);
			break;
		case QuadType::BottomLeft:
			bottomLeft->InsertRecursive(actor);
			break;
		case QuadType::BottomRight:
			bottomRight->InsertRecursive(actor);
			break;
		}
	}
	else
	{
		actors.emplace_back(actor);
	}
}

void QuadNode::QueryRecursive(const Bounds& bounds, std::vector<QuadNode*>& possibleNodes)
{
}

void QuadNode::DrawRecursive()
{
	if (!IsDivided())
	{
		if (!actors.empty())
		{
			Vector2<int> worldPos(bounds.x + 1, bounds.MaxY() - 1);
			Vector2<int> screenPos;
			if (TransformWorldToScreen(worldPos, screenPos))
			{
				Renderer::Instance().Submit(imgs[depth + 1], screenPos, WORD(Color::Yellow), depth);
			}
		}

		return;
	}

	// Draw Rect
	for (int x = bounds.x; x < bounds.MaxX(); ++x)
	{
		DrawPoint(Vector2<int>(x, bounds.y));
	}
	for (int y = bounds.y; y < bounds.MaxY(); ++y)
	{
		DrawPoint(Vector2<int>(bounds.x, y));
	}
	for (int x = bounds.x; x < bounds.MaxX(); ++x)
	{
		DrawPoint(Vector2<int>(x, bounds.MaxY()));
	}
	for (int y = bounds.y; y < bounds.MaxY(); ++y)
	{
		DrawPoint(Vector2<int>(bounds.MaxX(), y));
	}
	DrawPoint(Vector2<int>(bounds.MaxX(), bounds.MaxY()));

	// Draw Cross
	const int centerX = bounds.x + (bounds.w >> 1);
	const int centerY = bounds.y + (bounds.h >> 1);
	for (int x = bounds.x + 1; x < bounds.MaxX(); ++x)
	{
		DrawPoint(Vector2<int>(x, centerY));
	}
	for (int y = bounds.y + 1; y < bounds.MaxY(); ++y)
	{
		DrawPoint(Vector2<int>(centerX, y));
	}

	// Draw Depth Number
	DrawNumber(Vector2<int>(bounds.x + 1, bounds.MaxY() - 1));
	DrawNumber(Vector2<int>(bounds.x + 1 + centerX, bounds.MaxY() - 1));
	DrawNumber(Vector2<int>(bounds.x + 1, bounds.MaxY() - 1 - centerY));
	DrawNumber(Vector2<int>(bounds.x + 1 + centerX, bounds.MaxY() - 1 - centerY));

	topLeft->DrawRecursive();
	topRight->DrawRecursive();
	bottomLeft->DrawRecursive();
	bottomRight->DrawRecursive();
}

bool QuadNode::IsDivided()
{
	return topLeft != nullptr;
}

bool QuadNode::Subdivide()
{
	if (depth == maxDepth)
	{
		return false;
	}

	if (!IsDivided())
	{
		const int x = bounds.x;
		const int y = bounds.y;
		const int halfW = bounds.w >> 1;
		const int halfH = bounds.h >> 1;

		const int centerX = x + halfW;
		const int centerY = y + halfH;

		topLeft = std::make_unique<QuadNode>(Bounds{ x, y, halfW, halfH }, depth + 1);
		topRight = std::make_unique<QuadNode>(Bounds{ centerX, y, halfW, halfH }, depth + 1);
		bottomLeft = std::make_unique<QuadNode>(Bounds{ x, centerY, halfW, halfH }, depth + 1);
		bottomRight = std::make_unique<QuadNode>(Bounds{ centerX, centerY, halfW, halfH }, depth + 1);
	}

	return true;
}

std::vector<QuadType> QuadNode::GetQuadTypes(const Bounds& bounds)
{
	std::vector<QuadType> quads;

	const int x = this->bounds.x;
	const int y = this->bounds.y;
	const int halfW = this->bounds.w >> 1;
	const int halfH = this->bounds.h >> 1;

	const int centerX = x + halfW;
	const int centerY = y + halfH;

	bool left = bounds.x < centerX && bounds.MaxX() >= x;
	bool right = bounds.MaxX() >= centerX && bounds.x < this->bounds.MaxX();
	bool top = bounds.y < centerY && bounds.MaxY() >= y;
	bool bottom = bounds.MaxY() >= centerY && bounds.y < this->bounds.MaxY();

	if (top && left)
	{
		quads.emplace_back(QuadType::TopLeft);
	}

	if (top && right)
	{
		quads.emplace_back(QuadType::TopRight);
	}

	if (bottom && left)
	{
		quads.emplace_back(QuadType::BottomLeft);
	}

	if (bottom && right)
	{
		quads.emplace_back(QuadType::BottomRight);
	}

	return quads;
}

QuadType QuadNode::TestRegion(const Bounds& bounds)
{
	std::vector<QuadType> quads = GetQuadTypes(bounds);

	if (quads.empty())
	{
		return QuadType::OutOfArea;
	}

	if (quads.size() == 1)
	{
		return quads[0];
	}

	return QuadType::Straddling;
}

bool QuadNode::TransformWorldToScreen(const Vector2<int>& worldPos, Vector2<int>& outScreenPos)
{
	static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
	static const Vector2<int> mapHalfSize = mapSize / 2;
	static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

	// 1. world space -> view space
	Vector2<int> viewPos = worldPos + Renderer::Instance().GetViewTransform();

	// 2. veiw space -> screen space
	outScreenPos = viewPos;
	outScreenPos.y *= -1;
	outScreenPos += mapHalfSize;
	outScreenPos += mapStartPos;

	// 3. culling
	if (outScreenPos.x <= mapStartPos.x || outScreenPos.x >= mapStartPos.x + mapSize.x - 1
		|| outScreenPos.y <= mapStartPos.y || outScreenPos.y >= mapStartPos.y + mapSize.y - 1)
	{
		return false;
	}

	return true;
}

void QuadNode::DrawNumber(const Vector2<int>& worldPos)
{
	Vector2<int> screenPos;
	if (TransformWorldToScreen(worldPos, screenPos))
	{
		Renderer::Instance().Submit(imgs[depth + 1], screenPos, WORD(Color::Yellow), depth + 1);
	}
}

void QuadNode::DrawPoint(const Vector2<int>& worldPos)
{
	Vector2<int> screenPos;
	if (TransformWorldToScreen(worldPos, screenPos))
	{
		Renderer::Instance().Submit("o", screenPos, WORD(Color::DarkYellow), depth);
	}
}
