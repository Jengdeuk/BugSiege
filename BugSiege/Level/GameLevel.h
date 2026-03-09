#pragma once

#include "Level/Level.h"
#include "Common/Enums.h"

#include "Math/Vector2.h"
#include "Util/Timer.h"

#include "Actor/Tower/Tower.h"

namespace JD
{
	class Actor;
	template <typename T>
	class ObjectPool;
}

using namespace JD;

class PlayerController;

struct Bounds;
class QuadTree;

class SystemCore;
class CompilerTurret;
class DebuggerNode;
class GarbageCollector;
class MutexBarrier;
class ExceptionHandler;

class Bug;
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
	bool BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos, bool isForceCommand = false);
	void UpdateGridsForNavigation(const TowerType& type, const Vector2<int>& pos, const int value = 1);
	std::vector<Actor*> QueryActorsInRange(const Bounds& bounds);
	void RemoveActorInQuadTree(Actor* actor);

public:
	inline const Vector2<int>& GetMapSize() const { return mapSize; }
	const Tower::TowerData& GetTowerInitData(const TowerType& type) const;
	inline const std::vector<std::vector<int>>& GetDangerGrid() const { return dangerGrid; }
	inline const std::vector<std::vector<bool>>& GetWallGrid() const { return wallGrid; }
	inline const Vector2<int>& GetNextNodeByFlowField(const Vector2<int>& pos) const { return flowGrid[pos.y][pos.x]; }
	inline const std::vector<SystemCore*>& GetSystemCores() const { return systemCores; }

private:
	void DrawHUD();
	void DrawBorderLine();

private:
	void UpdateFlowGridByBFS();

private:
	Vector2<int> mapSize;
	float survivalTime = 0.0f;
	float lastDeltaTime = 0.0f;
	PlayerController* playerController = nullptr;
	std::vector<SystemCore*> systemCores;

// Navigation
private:
	std::vector<std::vector<int>> dangerGrid;
	std::vector<std::vector<bool>> wallGrid;
	std::vector<std::vector<Vector2<int>>> flowGrid;

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
	std::unique_ptr<ObjectPool<Bug>> bugPool;
	std::unique_ptr<ObjectPool<Segfault>> segfaultPool;

// HUD
private:
	char bufferCamPos[256] = {};
	char bufferMousePos[256] = {};
	char bufferStime[256] = {};
	char bufferFPS[256] = {};
};
