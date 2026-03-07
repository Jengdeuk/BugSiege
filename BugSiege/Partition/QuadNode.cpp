#include "QuadNode.h"

#include "Render/Renderer.h"
#include "Actor/Actor.h"
#include "Engine/Engine.h"

static const char* imgs[] = { "0", "1", "2", "3", "4", "5", "6" };
static const int maxActorsPerNode = 4;
static const int maxDepth = 6;

QuadNode::QuadNode(const Bounds& bounds, int depth)
	: bounds(bounds), depth(depth)
{
}

void QuadNode::InsertRecursive(Actor* actor)
{
	Vector2<int> actorPos = Vector2<int>(actor->GetPosition());
	QuadType type = TestRegion({ actorPos.x, actorPos.y, 1, 1 });
	if (type == QuadType::OutOfArea)
	{
		return;
	}

	if (IsDivided())
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
		actor->SetOwnerQuadNode(this);
		if (depth < maxDepth && actors.size() == maxActorsPerNode)
		{
			Subdivide();

			for (Actor* actor : actors)
			{
				actorPos = Vector2<int>(actor->GetPosition());
				type = TestRegion({ actorPos.x, actorPos.y, 1, 1 });
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

			actors.clear();
		}
	}
}

bool QuadNode::Remove(Actor* actor)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if (*it == actor)
		{
			actors.erase(it);
			return true;
		}
	}

	return false;
}

void QuadNode::QueryRecursive(const Bounds& bounds, std::vector<QuadNode*>& possibleNodes)
{
	possibleNodes.emplace_back(this);

	if (!IsDivided())
	{
		return;
	}

	std::vector<QuadType> quads = GetQuadTypes(bounds);
	for (const QuadType& index : quads)
	{
		switch (index)
		{
		case QuadType::TopLeft:
			topLeft->QueryRecursive(bounds, possibleNodes);
			break;
		case QuadType::TopRight:
			topRight->QueryRecursive(bounds, possibleNodes);
			break;
		case QuadType::BottomLeft:
			bottomLeft->QueryRecursive(bounds, possibleNodes);
			break;
		case QuadType::BottomRight:
			bottomRight->QueryRecursive(bounds, possibleNodes);
			break;
		default:
			break;
		}
	}
}

void QuadNode::DrawRecursive()
{
	if (!IsDivided())
	{
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
	DrawNumber(Vector2<int>(centerX + 1, bounds.MaxY() - 1));
	DrawNumber(Vector2<int>(bounds.x + 1, centerY - 1));
	DrawNumber(Vector2<int>(centerX + 1, centerY - 1));

	topLeft->DrawRecursive();
	topRight->DrawRecursive();
	bottomLeft->DrawRecursive();
	bottomRight->DrawRecursive();
}

bool QuadNode::CanMerge()
{
	if (topLeft->IsDivided() || topRight->IsDivided() || bottomLeft->IsDivided() || bottomRight->IsDivided())
	{
		return false;
	}

	size_t cnt = actors.size();
	cnt += topLeft->GetActors().size();
	cnt += topRight->GetActors().size();
	cnt += bottomLeft->GetActors().size();
	cnt += bottomRight->GetActors().size();

	return static_cast<int>(cnt) < maxActorsPerNode;
}

void QuadNode::MergeChildren()
{
	for (Actor* actor : topLeft->GetActors())
	{
		actors.emplace_back(actor);
		actor->SetOwnerQuadNode(this);
	}
	topLeft = nullptr;

	for (Actor* actor : topRight->GetActors())
	{
		actors.emplace_back(actor);
		actor->SetOwnerQuadNode(this);
	}
	topRight = nullptr;

	for (Actor* actor : bottomLeft->GetActors())
	{
		actors.emplace_back(actor);
		actor->SetOwnerQuadNode(this);
	}
	bottomLeft = nullptr;

	for (Actor* actor : bottomRight->GetActors())
	{
		actors.emplace_back(actor);
		actor->SetOwnerQuadNode(this);
	}
	bottomRight = nullptr;
}

bool QuadNode::IsDivided()
{
	return topLeft != nullptr;
}

void QuadNode::Subdivide()
{
	if (!IsDivided())
	{
		const int x = bounds.x;
		const int y = bounds.y;
		const int halfW = bounds.w >> 1;
		const int halfH = bounds.h >> 1;

		const int centerX = x + halfW;
		const int centerY = y + halfH;

		topLeft = std::make_unique<QuadNode>(Bounds{ x, centerY, halfW, halfH }, depth + 1);
		topRight = std::make_unique<QuadNode>(Bounds{ centerX, centerY, halfW, halfH }, depth + 1);
		bottomLeft = std::make_unique<QuadNode>(Bounds{ x, y, halfW, halfH }, depth + 1);
		bottomRight = std::make_unique<QuadNode>(Bounds{ centerX, y, halfW, halfH }, depth + 1);

		topLeft->parent = this;
		topRight->parent = this;
		bottomLeft->parent = this;
		bottomRight->parent = this;
	}
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

	bool left = bounds.x < centerX && bounds.MaxX() - 1 >= x;
	bool right = bounds.MaxX() - 1 >= centerX && bounds.x < this->bounds.MaxX();
	bool top = bounds.MaxY() - 1 >= centerY && bounds.y < this->bounds.MaxY();
	bool bottom = bounds.y < centerY && bounds.MaxY() - 1 >= y;

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
		Renderer::Instance().Submit(imgs[depth + 1], screenPos, WORD(Color::Cyan), 0);
	}
}

void QuadNode::DrawPoint(const Vector2<int>& worldPos)
{
	Vector2<int> screenPos;
	if (TransformWorldToScreen(worldPos, screenPos))
	{
		Renderer::Instance().Submit("#", screenPos, WORD(Color::DarkCyan), 0);
	}
}
