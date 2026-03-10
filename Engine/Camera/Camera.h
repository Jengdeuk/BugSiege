#pragma once

#include "Common/RTTI.h"

#include "Math/Color.h"
#include "Math/Vector2.h"

#include <memory>

namespace JD
{
	class JD_API Camera : public RTTI
	{
		RTTI_DECLARATIONS(Camera, RTTI)

	public:
		Camera(const float moveSpeed = 5.0f);

	public:
		void Move(const Vector2<float>& direction, float deltaTime);
		void Shake(const float shakeStrength);

	public:
		void SetPosition(const Vector2<float>& newPosition);
		void SetPositionToStart();
		void SetPositionBack();
		inline const Vector2<float>& GetPosition() const { return position; }

	private:
		float moveSpeed;
		Vector2<float> position;
		Vector2<float> startPos;
	};
}
