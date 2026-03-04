#pragma once

#include "Level/Level.h"

#include "Math/Vector2.h"
#include "Util/Timer.h"

namespace JD
{
	template <typename T>
	class ObjectPool;
}

using namespace JD;

class SystemCore;

class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel(const Vector2<int>& mapSize);

public:
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

public:
	inline const Vector2<int>& GetMapSize() const { return mapSize; }

private:
	void DrawHUD();
	void DrawBorderLine();

private:
	std::unique_ptr<ObjectPool<SystemCore>> systemCorePool;

private:
	Vector2<int> mapSize;

private:
	float survivalTime = 0.0f;
	float lastDeltaTime = 0.0f;

private:
	char buffer_camPos[256] = {};
	char buffer_stime[256] = {};
	char buffer_fps[256] = {};
};
