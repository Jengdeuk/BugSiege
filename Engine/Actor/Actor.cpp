#include "Actor.h"

#include "Render/Renderer.h"

#include "Engine/Engine.h"

namespace JD
{
	Actor::Actor(const InitData& initData)
		: initData(initData),
		position(initData.position),
		color(initData.color),
		sortingOrder(initData.sortingOrder)
	{
		if (initData.image == nullptr)
		{
			return;
		}

		size_t length = strlen(initData.image) + 1;
		this->image = std::make_unique<char[]>(length);
		std::memcpy(this->image.get(), initData.image, length);
	}

	Actor::~Actor()
	{
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		if (destroyRequested || !image.get())
		{
			return;
		}

		Vector2<int> screenPos;
		if (!TransformWorldToScreen(screenPos))
		{
			return;
		}

		Renderer::Instance().Submit(image.get(), screenPos, color, sortingOrder);
	}

	void Actor::TransformUpdate(float deltaTime)
	{
	}

	bool Actor::TransformWorldToScreen(Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapHalfSize = Engine::Instance().GetMapSize() / 2;

		Vector2<int> worldPos = Vector2<int>(position);

		// 1. world space -> view space
		Vector2<int> viewPos = worldPos + Renderer::Instance().GetViewTransform();

		// 2. culling
		if (viewPos.x <= -mapHalfSize.x || viewPos.x >= mapHalfSize.x - 1 || viewPos.y <= -mapHalfSize.y + 1 || viewPos.y >= mapHalfSize.y + 1)
		{
			return false;
		}

		// 3. veiw space -> screen space
		outScreenPos = viewPos;
		outScreenPos.y *= -1;
		outScreenPos += mapHalfSize;
		outScreenPos.y += 10;

		return true;
	}

	void Actor::Destroy()
	{
		destroyRequested = true;
	}

	void Actor::SetImage(const char* newImage)
	{
		size_t length = strlen(newImage) + 1;
		image = std::make_unique<char[]>(length);
		std::memcpy(image.get(), newImage, length);
	}

	void Actor::SetImage(std::unique_ptr<char[]> newImage)
	{
		image = std::move(newImage);
	}
}
