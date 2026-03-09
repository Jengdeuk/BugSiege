#include "GameLevel.h"

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Util/Util.h"

#include "Game/Game.h"
#include "Physics/PhysicsManager.h"
#include "Partition/UniformGrid.h"
#include "Partition/QuadTree.h"

#include "Actor/ObjectPool.h"
#include "Actor/Player/PlayerController.h"

#include "Actor/Tower/SystemCore.h"
#include "Actor/Tower/CompilerTurret.h"
#include "Actor/Tower/DebuggerNode.h"
#include "Actor/Tower/GarbageCollector.h"
#include "Actor/Tower/MutexBarrier.h"
#include "Actor/Tower/ExceptionHandler.h"

#include "Actor/Enemy/Bug.h"
#include "Actor/Enemy/Worm.h"
#include "Actor/Enemy/Trojan.h"
#include "Actor/Enemy/MemoryLeak.h"
#include "Actor/Enemy/Segfault.h"

#include <queue>

using namespace JD;

#define ANIMSEQ(seq) { seq, _countof(seq) }

static const Actor::AnimFrame damagedAnimSeqRGB[] =
{
	{ 0.06f, {}, Color::Red, {} },
	{ 0.06f, {}, Color::White, {} },
	{ 0.06f, {}, Color::Green, {} },
	{ 0.06f, {}, Color::White, {} },
	{ 0.06f, {}, Color::Blue, {} },
	{ 0.06f, {}, Color::White, {} },
	{ 0.06f, {}, Color::Red, {} },
	{ 0.06f, {}, Color::White, {} },
	{ 0.06f, {}, Color::Green, {} },
	{ 0.06f, {}, Color::White, {} },
	{ 0.06f, {}, Color::Blue, {} }
};

static const Actor::AnimFrame buildAnimSeqTower[] =
{
	{ 0.25f, "_", Color::Green, Color::Black },
	{ 0.25f, "=", Color::Green, Color::Black },
	{ 0.25f, " ", Color::Black, Color::Green }
};

static const Actor::AnimFrame buildAnimSeqSystemCore[] =
{
	{ 0.25f, "_", Color::Cyan, Color::Black },
	{ 0.25f, "=", Color::Cyan, Color::Black },
	{ 0.25f, " ", Color::Black, Color::Cyan }
};

static const Actor::AnimFrame attackAnimSeqTower[] =
{
	{ 0.1f, {}, {}, Color::DarkBlue }
};

static const Actor::AnimFrame collapsedAnimSeqTower[] =
{
	{ 0.06f, " ", Color::Black, Color::Green },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Green },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Green },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Green },
	{ 0.06f, "*", Color::Green, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Green, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black }
};

static const Actor::ActorData towerInitActorData[static_cast<int>(TowerType::Count)] =
{
	{ "@", {}, Color::Cyan, Color::Black, 11 },
	{ "C", {}, Color::Green, Color::Black, 11 },
	{ "D", {}, Color::Green, Color::Black, 11 },
	{ "G", {}, Color::Green, Color::Black, 11 },
	{ "M", {}, Color::Green, Color::Black, 11 },
	{ "E", {}, Color::Green, Color::Black, 11 }
};

static const Tower::TowerData towerInitTowerData[static_cast<int>(TowerType::Count)] =
{
	{ // SystemCore
		0, 0, 0, 0.0f, 0.0f,
		ANIMSEQ(buildAnimSeqSystemCore),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	},
	
	{ // CompilerTurret
		1, 10, 1, 4.5f, 0.5f,
		ANIMSEQ(buildAnimSeqTower),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	},
	
	{ // DebuggerNode
		2, 20, 1, 7.0f, 0.25f,
		ANIMSEQ(buildAnimSeqTower),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	},
	
	{ // GarbageCollector
		3, 30, 3, 3.5f, 3.0f,
		ANIMSEQ(buildAnimSeqTower),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	},
	
	{ // MutexBarrier
		4, 40, 1, 6.0f, 5.0f,
		ANIMSEQ(buildAnimSeqTower),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	},
	
	{ // ExceptionHandler
		5, 50, 1, 5.0f, 1.0f,
		ANIMSEQ(buildAnimSeqTower),
		ANIMSEQ(attackAnimSeqTower),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(collapsedAnimSeqTower)
	}
};

static const Actor::AnimFrame occurAnimSeqEnemy[] =
{
	{ 0.06f, "*", Color::Red, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Red, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red }
};

static const Actor::AnimFrame occurAnimSeqSegfault[] =
{
	{ 0.06f, "*", Color::Magenta, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Magenta, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta }
};

static const Actor::AnimFrame attackAnimSeqEnemy[] =
{
	{ 0.1f, {}, {}, Color::DarkRed }
};

static const Actor::AnimFrame attackAnimSeqSegfault[] =
{
	{ 0.1f, {}, {}, Color::DarkMagenta }
};

static const Actor::AnimFrame fixedAnimSeqEnemy[] =
{
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Red },
	{ 0.06f, "*", Color::Red, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Red, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black }
};

static const Actor::AnimFrame fixedAnimSeqSegfault[] =
{
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, " ", Color::Black, Color::White },
	{ 0.06f, " ", Color::Black, Color::Magenta },
	{ 0.06f, "*", Color::Magenta, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Magenta, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black }
};

static const Actor::ActorData enemyInitActorData[static_cast<int>(EnemyType::Count)] =
{
	{ "b", {}, Color::Red, Color::Black, 5 },
	{ "w", {}, Color::Red, Color::Black, 6 },
	{ "t", {}, Color::Red, Color::Black, 7 },
	{ "L", {}, Color::Red, Color::Black, 8 },
	{ "S", {}, Color::Magenta, Color::Black, 9 }
};

static const Enemy::EnemyData enemyInitEnemyData[static_cast<int>(EnemyType::Count)] =
{
	{ // Bug
		1, 1, 5, 1, 1.15f, 1.2f, 6.0f,
		ANIMSEQ(occurAnimSeqEnemy),
		ANIMSEQ(attackAnimSeqEnemy),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(fixedAnimSeqEnemy)
	},

	{ // Worm
		2, 2, 4, 1, 3.5f, 0.75f, 15.0f,
		ANIMSEQ(occurAnimSeqEnemy),
		ANIMSEQ(attackAnimSeqEnemy),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(fixedAnimSeqEnemy)
	},

	{ // Trojan
		3, 3, 20, 5, 1.1f, 3.0f, 2.0f,
		ANIMSEQ(occurAnimSeqEnemy),
		ANIMSEQ(attackAnimSeqEnemy),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(fixedAnimSeqEnemy)
	},

	{ // MemoryLeak
		4, 4, 8, 1, 5.1f, 3.0f, 4.5f,
		ANIMSEQ(occurAnimSeqEnemy),
		ANIMSEQ(attackAnimSeqEnemy),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(fixedAnimSeqEnemy)
	},

	{ // Segfault
		5, 5, 30, 10, 2.1f, 0.35f, 7.0f,
		ANIMSEQ(occurAnimSeqSegfault),
		ANIMSEQ(attackAnimSeqSegfault),
		ANIMSEQ(damagedAnimSeqRGB),
		ANIMSEQ(fixedAnimSeqSegfault)
	}
};

GameLevel::GameLevel(const Vector2<int>& mapSize)
	: mapSize(mapSize)
{
	Initialize();
}

void GameLevel::Initialize()
{
	isGameOver = false;
	integrity = 100;
	cpu = 10;
	survivalTime = 0.0f;
	lastDeltaTime = 0.0f;

	regenTime = 5.0f;
	regenCount = 5.0f;
	regenTimer.Reset();
	regenTimer.SetTargetTime(regenTime);

	level = 1;
	levelUpTime = 15.0f;
	levelUpTimer.Reset();
	levelUpTimer.SetTargetTime(levelUpTime);

	// Navigation & Partition
	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	dangerGrid.assign(gridSize.y, std::vector<int>(gridSize.x, 0));
	wallGrid.assign(gridSize.y, std::vector<bool>(gridSize.x, false));
	flowGrid.assign(gridSize.y, std::vector<Vector2<int>>(gridSize.x, Vector2<int>()));
	cellSize = 8;
	isDrawDebugQuadTree = false;
	uniformGrid = std::make_unique<UniformGrid>(cellSize);
	quadTree = std::make_unique<QuadTree>();

	// Player
	std::unique_ptr<PlayerController> newPlayerController = std::make_unique<PlayerController>();
	playerController = newPlayerController.get();
	AddNewActor(std::move(newPlayerController));

	// Object Pool
	systemCorePool = std::make_unique<ObjectPool<SystemCore>>();
	compilerTurretPool = std::make_unique<ObjectPool<CompilerTurret>>();
	debuggerNodePool = std::make_unique<ObjectPool<DebuggerNode>>();
	garbageCollectorPool = std::make_unique<ObjectPool<GarbageCollector>>();
	mutexBarrierPool = std::make_unique<ObjectPool<MutexBarrier>>();
	exceptionHandlerPool = std::make_unique<ObjectPool<ExceptionHandler>>();
	bugPool = std::make_unique<ObjectPool<Bug>>();
	wormPool = std::make_unique<ObjectPool<Worm>>();
	trojanPool = std::make_unique<ObjectPool<Trojan>>();
	memoryLeakPool = std::make_unique<ObjectPool<MemoryLeak>>();
	segfaultPool = std::make_unique<ObjectPool<Segfault>>();

	// System Core
	float centerX = static_cast<float>((gridSize.x - 1) / 2);
	float centerY = static_cast<float>((gridSize.y - 1) / 2);
	const int dx[] = { 0, 1, 0, 1 };
	const int dy[] = { 0, 0, 1, 1 };
	for (int i = 0; i < 4; ++i)
	{
		BuildTowerToGround(TowerType::SystemCore, Vector2<float>(centerX + dx[i], centerY + dy[i]), true);
	}
}

void GameLevel::Tick(float deltaTime)
{
	if (Input::Instance().GetKeyDown(VK_ESCAPE))
	{
		Game::Instance().ToggleMenu();
		return;
	}

	if (Input::Instance().GetKeyDown('0'))
	{
		isDrawDebugQuadTree = !isDrawDebugQuadTree;
		return;
	}

	if (isGameOver)
	{
		return;
	}

	Super::Tick(deltaTime);
	PhysicsManager::Instance().UpdatePhysics(Super::GetActors(), deltaTime);

	levelUpTimer.Tick(deltaTime);
	if (levelUpTimer.IsTimeOut())
	{
		levelUpTimer.Reset();
		LevelUp();
	}

	regenTimer.Tick(deltaTime);
	if (regenTimer.IsTimeOut())
	{
		regenTimer.Reset();
		Regen();
	}

	survivalTime += deltaTime;
	lastDeltaTime = deltaTime;
}

void GameLevel::Draw()
{
	Super::Draw();

	if (isDrawDebugQuadTree)
	{
		quadTree->Draw();
	}

	DrawHUD();
}

void GameLevel::DamagedSystemCore(const int damage)
{
	integrity -= damage;
	if (integrity <= 0)
	{
		integrity = 0;
		isGameOver = true;
	}
}

void GameLevel::GainCPU(const int amount)
{
	cpu += amount;
}

bool GameLevel::BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos, bool isForceCommand)
{
	const int typeIdx = static_cast<int>(type);
	if (cpu < towerInitTowerData[typeIdx].tier)
	{
		return false;
	}

	Vector2<int> screenPos;
	if (!isForceCommand && !Renderer::Instance().TransformWorldToScreen(groundPos, screenPos))
	{
		return false;
	}

	const Vector2<int> pos{ groundPos };
	if (wallGrid[pos.y][pos.x])
	{
		return false;
	}

	// Acquire and Add Tower to Level
	std::unique_ptr<Tower> tower;
	switch (type)
	{
	case TowerType::SystemCore:
		tower = systemCorePool->Acquire();
		systemCores.emplace_back(tower->As<SystemCore>());
		break;
	case TowerType::CompilerTurret:
		tower = compilerTurretPool->Acquire();
		break;
	case TowerType::DebuggerNode:
		tower = debuggerNodePool->Acquire();
		break;
	case TowerType::GarbageCollector:
		tower = garbageCollectorPool->Acquire();
		break;
	case TowerType::MutexBarrier:
		tower = mutexBarrierPool->Acquire();
		break;
	case TowerType::ExceptionHandler:
		tower = exceptionHandlerPool->Acquire();
		break;
	default:
		return false;
	}

	Actor* towerActor = tower->As<Actor>();
	Actor::ActorData actorData{ towerInitActorData[typeIdx] };
	actorData.position = groundPos;
	towerActor->Initialize(actorData);
	tower->Initialize(towerInitTowerData[typeIdx]);
	AddNewActor(std::move(tower));

	// Update Grids for Navigation
	UpdateGridsForNavigation(type, pos, 1);

	// Insert Tower to QuadTree
	if (type != TowerType::SystemCore)
	{
		quadTree->Insert(towerActor);
	}

	cpu -= towerInitTowerData[typeIdx].tier;
	
	return true;
}

void GameLevel::UpdateGridsForNavigation(const TowerType& type, const Vector2<int>& pos, const int value)
{
	// Wall Grid
	wallGrid[pos.y][pos.x] = (value == 1);

	// Danger Grid
	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	const int r = static_cast<int>(towerInitTowerData[static_cast<int>(type)].radius);
	for (int y = pos.y - r; y <= pos.y + r; ++y)
	{
		for (int x = pos.x - r; x <= pos.x + r; ++x)
		{
			if (x < 0 || x >= gridSize.x || y < 0 || y >= gridSize.y)
			{
				continue;
			}

			int dx = x - pos.x;
			int dy = y - pos.y;
			if (dx * dx + dy * dy <= r * r)
			{
				dangerGrid[y][x] += value;
			}
		}
	}

	// Flow Grid
	//UpdateFlowGridByBFS();
}

std::vector<Actor*> GameLevel::QueryActorsInQuadTree(const Bounds& bounds)
{
	return quadTree->Query(bounds);
}

void GameLevel::RemoveActorInQuadTree(Actor* actor)
{
	quadTree->Remove(actor);
}

std::vector<Actor*> GameLevel::QueryActorsInUniformGrid(const Vector2<int>& pos, const float r)
{
	return uniformGrid->Query(pos, r);
}

void GameLevel::InsertActorInUniformGrid(const Vector2<int>& cellPos, Actor* actor)
{
	uniformGrid->Insert(cellPos, actor);
}

void GameLevel::RemoveActorInUniformGrid(const Vector2<int>& cellPos, Actor* actor)
{
	uniformGrid->Remove(cellPos, actor);
}

const Tower::TowerData& GameLevel::GetTowerInitData(const TowerType& type) const
{
	return towerInitTowerData[static_cast<int>(type)];
}

void GameLevel::DrawHUD()
{
	static const int screenX = Engine::Instance().GetScreenSize().x;
	static const int screenY = Engine::Instance().GetScreenSize().y;

	// wave
	Renderer::Instance().Submit("WAVE: ", Vector2<int>(screenX - 21, 2), Color::White);
	sprintf_s(bufferLevel, "%02d", level);
	Renderer::Instance().Submit(bufferLevel, Vector2<int>(screenX - 15, 2), Color::Red);

	// time
	Renderer::Instance().Submit("TIME: ", Vector2<int>(screenX - 12, 2), Color::White);
	sprintf_s(bufferStime, "%02d:%02d", static_cast<int>(survivalTime / 60), static_cast<int>(survivalTime) % 60);
	Renderer::Instance().Submit(bufferStime, Vector2<int>(screenX - 6, 2), Color::Gray);

	// integrity
	Renderer::Instance().Submit("INTEGRITY: ", Vector2<int>(1, 2), Color::White);
	sprintf_s(bufferIntegrity, "%d%%", integrity);
	Renderer::Instance().Submit(bufferIntegrity, Vector2<int>(12, 2), Color::Cyan);

	// CPU
	Renderer::Instance().Submit("CPU: ", Vector2<int>(1, 4), Color::White);
	sprintf_s(bufferCPU, "%d", cpu);
	Renderer::Instance().Submit(bufferCPU, Vector2<int>(6, 4), Color::Green);

	// Build
	TowerType selectedTowerTypeToBuild = playerController->GetSelectedTowerTypeToBuild();
	Renderer::Instance().Submit("Build: ", Vector2<int>(1, 5), Color::White);
	Renderer::Instance().Submit("[1]", Vector2<int>(8, 5), Color::Green);
	Renderer::Instance().Submit("Compiler", Vector2<int>(11, 5), (selectedTowerTypeToBuild == TowerType::CompilerTurret ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[2]", Vector2<int>(20, 5), Color::Green);
	Renderer::Instance().Submit("Debugger", Vector2<int>(23, 5), (selectedTowerTypeToBuild == TowerType::DebuggerNode ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[3]", Vector2<int>(32, 5), Color::Green);
	Renderer::Instance().Submit("GC", Vector2<int>(35, 5), (selectedTowerTypeToBuild == TowerType::GarbageCollector ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[4]", Vector2<int>(38, 5), Color::Green);
	Renderer::Instance().Submit("Mutex", Vector2<int>(41, 5), (selectedTowerTypeToBuild == TowerType::MutexBarrier ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[5]", Vector2<int>(47, 5), Color::Green);
	Renderer::Instance().Submit("Except", Vector2<int>(50, 5), (selectedTowerTypeToBuild == TowerType::ExceptionHandler ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[R]Cancel", Vector2<int>(57, 5), Color::Gray);

	// camPos
	const Vector2<int> viewTransform = Renderer::Instance().GetViewTransform() * -1;
	sprintf_s(bufferCamPos, "Camera:(%d, %d)", viewTransform.x, viewTransform.y);
	Renderer::Instance().Submit(bufferCamPos, Vector2<int>(1, 7), Color::DarkGray);

	// mousePos
	const Vector2<int> mousePosition{ Input::Instance().MouseWorldPosition() };
	sprintf_s(bufferMousePos, "Cursor:(%d, %d)", mousePosition.x, mousePosition.y);
	Renderer::Instance().Submit(bufferMousePos, Vector2<int>(1, 8), Color::DarkGray);

	// Other
	Renderer::Instance().Submit("Action: ", Vector2<int>(1, 9), Color::White);
	Renderer::Instance().Submit("[Cursor, W, A, S, D]", Vector2<int>(9, 9), Color::DarkCyan);
	Renderer::Instance().Submit("ControlCamera", Vector2<int>(29, 9), Color::Gray);
	Renderer::Instance().Submit("[Space]", Vector2<int>(43, 9), Color::DarkCyan);
	Renderer::Instance().Submit("JumpToSystemCore", Vector2<int>(50, 9), Color::Gray);

	// fps
	sprintf_s(bufferFPS, "FPS:%d", static_cast<int>(1.0f / lastDeltaTime));
	Renderer::Instance().Submit(bufferFPS, Vector2<int>(screenX - 8, 9), Color::DarkGray);

	DrawBorderLine();
}

void GameLevel::DrawBorderLine()
{
	static const int screenX = Engine::Instance().GetScreenSize().x;
	static const int screenY = Engine::Instance().GetScreenSize().y;

	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 0), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 1), Color::Gray);
	Renderer::Instance().Submit("Terminal Defense: Bug Siege", Vector2<int>(1, 1), Color::DarkCyan);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 1), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 2), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 2), Color::Gray);
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 3), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 4), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 4), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 5), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 5), Color::Gray);
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 6), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 7), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 7), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 8), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 8), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 9), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 9), Color::Gray);
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 10), Color::Gray);
	for (int i = 11; i < screenY - 1; ++i)
	{
		Renderer::Instance().Submit("|", Vector2<int>(0, i), Color::Gray);
		Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, i), Color::Gray);
	}
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, screenY - 1), Color::Gray);
}

void GameLevel::UpdateFlowGridByBFS()
{
	using p = std::pair<int, Vector2<int>>;

	for (auto& row : flowGrid)
	{
		for (auto& node : row)
		{
			node = Vector2<int>();
		}
	}

	static const int inf = INT_MAX;
	static const int dx[] = { -1, 0, 1, 0 };
	static const int dy[] = { 0, -1, 0, 1 };

	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	std::vector<std::vector<int>> distGrid(gridSize.y, std::vector<int>(gridSize.x, inf));

	std::queue<p> q;
	for (SystemCore* systemCore : systemCores)
	{
		Vector2<int> pos{ systemCore->GetPosition() };
		distGrid[pos.y][pos.x] = 0;
		q.emplace(0, pos);
	}

	while (!q.empty())
	{
		int dist = q.front().first;
		Vector2<int> pos = q.front().second;
		q.pop();

		if (dist > distGrid[pos.y][pos.x])
		{
			continue;
		}

		for (int i = 0; i < 4; ++i)
		{
			int nx = pos.x + dx[i];
			int ny = pos.y + dy[i];
			if (nx < 0 || nx >= gridSize.x || ny < 0 || ny >= gridSize.y || wallGrid[ny][nx])
			{
				continue;
			}
			
			int ndist = dist + 1;
			if (ndist < distGrid[ny][nx])
			{
				distGrid[ny][nx] = ndist;
				flowGrid[ny][nx] = pos;
				q.emplace(ndist, Vector2<int>(nx, ny));
			}
		}
	}
}

void GameLevel::Regen()
{
	static const Vector2<float> gridSize{ Engine::Instance().GetGridSize() };

	int typeIdx;
	std::unique_ptr<Enemy> newEnemy;
	Actor::ActorData actorData;

	for (int i = 0; i < regenCount; ++i)
	{
		const float rv = Util::Randomf(0.0f, 100.0f);
		const float rx = Util::Randomf(0.0f, gridSize.x -1.0f);
		const float ry = Util::Randomf(0.0f, gridSize.y -1.0f);

		if (rv >= 75.0f)
		{
			typeIdx = static_cast<int>(EnemyType::Bug);
			newEnemy = bugPool->Acquire();
		}
		else if (rv >= 50.0f)
		{
			typeIdx = static_cast<int>(EnemyType::Worm);
			newEnemy = wormPool->Acquire();
		}
		else if (rv >= 30.0f)
		{
			typeIdx = static_cast<int>(EnemyType::Trojan);
			newEnemy = trojanPool->Acquire();
		}
		else if (rv >= 10.0f)
		{
			typeIdx = static_cast<int>(EnemyType::MemoryLeak);
			newEnemy = memoryLeakPool->Acquire();
		}
		else
		{
			typeIdx = static_cast<int>(EnemyType::Segfault);
			newEnemy = segfaultPool->Acquire();
		}

		actorData = enemyInitActorData[typeIdx];
		actorData.position = { rx, ry };
		newEnemy->As<Actor>()->Initialize(actorData);
		newEnemy->Initialize(enemyInitEnemyData[typeIdx]);
		AddNewActor(std::move(newEnemy));
	}
}

void GameLevel::LevelUp()
{
	++level;

	//regenTime *= 0.9f;
	regenCount *= 1.2f;

	regenTimer.Reset();
	regenTimer.SetTargetTime(regenTime);
}
