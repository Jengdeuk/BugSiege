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
class UniformGrid;

class SystemCore;
class CompilerTurret;
class DebuggerNode;
class GarbageCollector;
class MutexBarrier;
class ExceptionHandler;

class Bug;
class Worm;
class Trojan;
class MemoryLeak;
class MemoryLeakSmall;
class Segfault;

class ExplosionEffect;

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
	void DamagedSystemCore(const int damage);
	void GainCPU(const int amount);
	void SummonMemoryLeakSmalls(const Vector2<float>& pos);
	void SummonExplosionEffect(const Vector2<float>& pos);
	bool BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos, bool isForceCommand = false);
	void UpdateGridsForNavigation(const TowerType& type, const Vector2<int>& pos, const int value = 1);
	std::vector<Actor*> QueryActorsInQuadTree(const Bounds& bounds);
	void RemoveActorInQuadTree(Actor* actor);
	std::vector<Actor*> QueryActorsInUniformGrid(const Vector2<int>& pos, const float r);
	void InsertActorInUniformGrid(const Vector2<int>& cellPos, Actor* actor);
	void RemoveActorInUniformGrid(const Vector2<int>& cellPos, Actor* actor);

public:
	inline const Vector2<int>& GetMapSize() const { return mapSize; }
	inline int GetCellSize() const { return cellSize; }
	inline const std::vector<std::vector<int>>& GetDangerGrid() const { return dangerGrid; }
	inline const std::vector<std::vector<bool>>& GetWallGrid() const { return wallGrid; }
	inline const Vector2<int>& GetNextNodeByFlowField(const Vector2<int>& pos) const { return flowGrid[pos.y][pos.x]; }
	inline const std::vector<SystemCore*>& GetSystemCores() const { return systemCores; }

public:
	const Tower::TowerData& GetTowerInitData(const TowerType& type) const;

private:
	void DrawBackground();
	void DrawHUD();
	void DrawBorderLine();

private:
	void UpdateFlowGridByBFS();
	void Regen();
	void LevelUp();

private:
	Vector2<int> mapSize;
	bool isGameOver = false;
	int integrity = 0;
	int cpu = 0;
	float survivalTime = 0.0f;
	float lastDeltaTime = 0.0f;
	PlayerController* playerController = nullptr;
	std::vector<SystemCore*> systemCores;

private:
	float regenTime = 0.0f;
	float regenCount = 0.0f;
	float regenRadius = 0.0f;
	Timer regenTimer;
	int level = 0;
	float levelUpTime = 0.0f;
	Timer levelUpTimer;

private:
	bool hasDamaged = false;
	float damagedAnimTime = 0.0f;
	Timer damagedAnimTimer;

// Navigation
private:
	std::vector<std::vector<int>> dangerGrid;
	std::vector<std::vector<bool>> wallGrid;
	std::vector<std::vector<Vector2<int>>> flowGrid;

// Partition
private:
	bool isDrawDebugQuadTree = false;
	int cellSize = 0;
	std::unique_ptr<QuadTree> quadTree;
	std::unique_ptr<UniformGrid> uniformGrid;

// ObjectPool
private:
	std::unique_ptr<ObjectPool<SystemCore>> systemCorePool;
	std::unique_ptr<ObjectPool<CompilerTurret>> compilerTurretPool;
	std::unique_ptr<ObjectPool<DebuggerNode>> debuggerNodePool;
	std::unique_ptr<ObjectPool<GarbageCollector>> garbageCollectorPool;
	std::unique_ptr<ObjectPool<MutexBarrier>> mutexBarrierPool;
	std::unique_ptr<ObjectPool<ExceptionHandler>> exceptionHandlerPool;
	std::unique_ptr<ObjectPool<Bug>> bugPool;
	std::unique_ptr<ObjectPool<Worm>> wormPool;
	std::unique_ptr<ObjectPool<Trojan>> trojanPool;
	std::unique_ptr<ObjectPool<MemoryLeak>> memoryLeakPool;
	std::unique_ptr<ObjectPool<MemoryLeakSmall>> memoryLeakSmallPool;
	std::unique_ptr<ObjectPool<Segfault>> segfaultPool;
	std::unique_ptr<ObjectPool<ExplosionEffect>> explosionEffectPool;

// HUD
private:
	char bufferIntegrity[10] = {};
	char bufferCPU[10] = {};
	char bufferLevel[10] = {};
	char bufferStime[20] = {};
	char bufferCamPos[20] = {};
	char bufferMousePos[20] = {};
	char bufferFPS[10] = {};
};
