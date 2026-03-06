#include "Camera.h"

#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include "Util/Util.h"

namespace JD
{
	Camera::Camera(const float moveSpeed)
		: moveSpeed(moveSpeed)
	{
		startPos =
		{
			(Engine::Instance().GetGridSize().x - 1.0f) / 2.0f,
			(Engine::Instance().GetGridSize().y - 1.0f) / 2.0f
		};

		position = startPos;
	}

	void Camera::SetPosition(const Vector2<float>& newPosition)
	{
		position = newPosition;

		Renderer::Instance().SetViewTransform(Vector2<int>(position * -1.0f));
	}

	void Camera::SetPositionToStart()
	{
		position = startPos;

		Renderer::Instance().SetViewTransform(Vector2<int>(position * -1.0f));
	}

	void Camera::Move(const Vector2<float>& direction, float deltaTime)
	{
		static const Vector2<float> gridSize{ Engine::Instance().GetGridSize() };
		static const Vector2<float> mapHalfSize = Vector2<float>(Engine::Instance().GetMapSize()) / 2.0f;

		position += direction * moveSpeed * deltaTime;
		position.x = Util::Clamp(position.x, mapHalfSize.x - 1, gridSize.x - mapHalfSize.x + 1);
		position.y = Util::Clamp(position.y, mapHalfSize.y - 2, gridSize.y - mapHalfSize.y);

		Renderer::Instance().SetViewTransform(Vector2<int>(position * -1.0f));
	}
}