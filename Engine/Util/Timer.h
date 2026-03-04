#pragma once

#include "Common/Common.h"

namespace JD
{
	class JD_API Timer
	{
	public:
		Timer(float targetTime = 1.0f);

	public:
		void Tick(float deltaTime);

	public:
		void Reset();
		bool IsTimeOut() const;
		void SetTargetTime(float newTargetTime);
		inline const float GetTargetTime() const { return targetTime; }
		inline const float GetElapsedTime() const { return elapsedTime; }

	private:
		float elapsedTime = 0.0f;
		float targetTime = 0.0f;
	};
}
