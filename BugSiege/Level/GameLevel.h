#pragma once

#include "Level/Level.h"
#include "Common/Enums.h"

#include "Math/Vector2.h"
#include "Util/Timer.h"

#include "Actor/Tower/Tower.h"

namespace JD
{
	template <typename T>
	class ObjectPool;
}

using namespace JD;

class QuadTree;
class SystemCore;
class CompilerTurret;
class DebuggerNode;
class GarbageCollector;
class MutexBarrier;
class ExceptionHandler;
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
	const Tower::TowerData& GetTowerInitData(const TowerType& type) const;

private:
	void DrawHUD();
	void DrawBorderLine();

private:
	Vector2<int> mapSize;

private:
	float survivalTime = 0.0f;
	float lastDeltaTime = 0.0f;

// Navigation
private:
	Segfault* segfault = nullptr;
	std::vector<std::vector<int>> dangerGrid;
	std::vector<std::vector<bool>> wallGrid;

// Partition
private:
	std::unique_ptr<QuadTree> quadTree;

// ObjectPool
private:
	std::unique_ptr<ObjectPool<SystemCore>> systemCorePool;
	std::unique_ptr<ObjectPool<CompilerTurret>> compilerTurretPool;
	std::unique_ptr<ObjectPool<DebuggerNode>> debuggerNodePool;
	std::unique_ptr<ObjectPool<GarbageCollector>> garbageCollectorPool;
	std::unique_ptr<ObjectPool<MutexBarrier>> mutexBarrierPool;
	std::unique_ptr<ObjectPool<ExceptionHandler>> exceptionHandlerPool;
	std::unique_ptr<ObjectPool<Segfault>> segfaultPool;

// HUD
private:
	char bufferCamPos[256] = {};
	char bufferMousePos[256] = {};
	char bufferStime[256] = {};
	char bufferFPS[256] = {};
};
