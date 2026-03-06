#pragma once

#include "Level/Level.h"
#include "Common/Enums.h"

#include "Math/Vector2.h"
#include "Util/Timer.h"

namespace JD
{
	template <typename T>
	class ObjectPool;
}

using namespace JD;

class SystemCore;
class CompilerTurret;

class Segfault;

class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel(const Vector2<int>& mapSize);

public:
	void Initialize();
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

public:
	bool BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos);

public:
	inline const Vector2<int>& GetMapSize() const { return mapSize; }

private:
	void DrawHUD();
	void DrawBorderLine();

private:
	Vector2<int> mapSize;

private:
	float survivalTime = 0.0f;
	float lastDeltaTime = 0.0f;

// Grid
private:
	Segfault* segfault = nullptr;
	std::vector<std::vector<int>> dangerGrid;
	std::vector<std::vector<bool>> wallGrid;

// ObjectPool
private:
	std::unique_ptr<ObjectPool<SystemCore>> systemCorePool;
	std::unique_ptr<ObjectPool<CompilerTurret>> compilerTurretPool;
	std::unique_ptr<ObjectPool<Segfault>> segfaultPool;

// HUD
private:
	char bufferCamPos[256] = {};
	char bufferMousePos[256] = {};
	char bufferStime[256] = {};
	char bufferFPS[256] = {};
};
