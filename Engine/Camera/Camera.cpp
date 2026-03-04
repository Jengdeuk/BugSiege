#include "Camera.h"

#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include "Util/Util.h"

namespace JD
{
	static const Vector2<float> gridSize{ 256.0f, 256.0f };
	static const Vector2<float> startPos{ (gridSize.x - 1.0f) / 2.0f, (gridSize.y - 1) / 2.0f };

	Camera::Camera(const float moveSpeed)
		: moveSpeed(moveSpeed)
	{
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
		static const float mapXH = (static_cast<float>(Engine::Instance().GetMapSize().x) - 2.0f) / 2.0f;
		static const float mapYH = (static_cast<float>(Engine::Instance().GetMapSize().y) - 2.0f) / 2.0f;

		position += direction * moveSpeed * deltaTime;

		position.x = Util::Clamp(position.x, mapXH, gridSize.x - mapXH);
		position.y = Util::Clamp(position.y, mapYH - 1.0f, gridSize.y - (mapYH + 1.0f) - 1.0f);

		Renderer::Instance().SetViewTransform(Vector2<int>(position * -1.0f));
	}
}