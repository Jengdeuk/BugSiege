#include "Actor.h"

#include "Render/Renderer.h"

#include "Engine/Engine.h"

namespace JD
{
	void Actor::Initialize(const ActorData& initData)
	{
		hasBeganPlay = false;

		actorData = initData;
		position = initData.position;
		color = initData.color;
		sortingOrder = initData.sortingOrder;
		if (actorData.image)
		{
			size_t length = strlen(actorData.image) + 1;
			this->image = std::make_unique<char[]>(length);
			std::memcpy(this->image.get(), actorData.image, length);
		}
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
		destroyRequested = false;
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
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		Vector2<int> worldPos = Vector2<int>(position);

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

	bool Actor::TransformWorldToScreen(const Vector2<float>& worldPosf, Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		Vector2<int> worldPos = Vector2<int>(worldPosf);

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
