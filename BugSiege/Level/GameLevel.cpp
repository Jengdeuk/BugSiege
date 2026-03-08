#include "GameLevel.h"

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Util/Util.h"

#include "Game/Game.h"
#include "Physics/PhysicsManager.h"
#include "Partition/QuadTree.h"

#include "Actor/ObjectPool.h"
#include "Actor/Player/PlayerController.h"

#include "Actor/Tower/SystemCore.h"
#include "Actor/Tower/CompilerTurret.h"
#include "Actor/Tower/DebuggerNode.h"
#include "Actor/Tower/GarbageCollector.h"
#include "Actor/Tower/MutexBarrier.h"
#include "Actor/Tower/ExceptionHandler.h"

#include "Actor/Enemy/Segfault.h"

#include <memory>

using namespace JD;

static const Actor::AnimFrame buildAnimSeqTower[] =
{
	{ 0.25f, "_", Color::Green, false },
	{ 0.25f, "=", Color::Green, false },
	{ 0.25f, " ", Color::Green << 4, false }
};

static const Actor::AnimFrame collapsedAnimSeqTower[] =
{
	{ 0.06f, "*", Color::Green, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, "*", Color::Green, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, " ", Color::Green << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Green << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Green << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Green << 4, false }
};

static const Actor::AnimFrame buildAnimSeqSystemCore[] =
{
	{ 0.25f, "_", Color::Cyan, false },
	{ 0.25f, "=", Color::Cyan, false },
	{ 0.25f, " ", Color::Cyan << 4, false }
};

static const Actor::ActorData towerInitActorData[static_cast<int>(TowerType::Count)] =
{
	{ "@", {}, Color::Cyan, 11 },
	{ "C", {}, Color::Green, 11 },
	{ "D", {}, Color::Green, 11 },
	{ "G", {}, Color::Green, 11 },
	{ "M", {}, Color::Green, 11 },
	{ "E", {}, Color::Green, 11 }
};

static const Tower::TowerData towerInitTowerData[static_cast<int>(TowerType::Count)] =
{
	{ 0, 0, 0, 0.0f, 0.0f, { buildAnimSeqSystemCore, _countof(buildAnimSeqSystemCore) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } },
	{ 1, 1, 1, 4.0f, 0.25f, { buildAnimSeqTower, _countof(buildAnimSeqTower) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } },
	{ 2, 1, 1, 6.0f, 0.25f, { buildAnimSeqTower, _countof(buildAnimSeqTower) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } },
	{ 3, 1, 3, 3.0f, 3.0f, { buildAnimSeqTower, _countof(buildAnimSeqTower) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } },
	{ 4, 1, 1, 5.0f, 5.0f, { buildAnimSeqTower, _countof(buildAnimSeqTower) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } },
	{ 5, 1, 1, 4.0f, 1.0f, { buildAnimSeqTower, _countof(buildAnimSeqTower) }, { collapsedAnimSeqTower, _countof(collapsedAnimSeqTower) } }
};

static const Actor::AnimFrame occurAnimSeqEnmey[] =
{
	{ 0.06f, "*", Color::Red, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, "*", Color::Red, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, " ", Color::Red << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Red << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Red << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Red << 4, false }
};

static const Actor::AnimFrame occurAnimSeqSegfault[] =
{
	{ 0.06f, "*", Color::Magenta, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, "*", Color::Magenta, false },
	{ 0.06f, "*", Color::White, false },
	{ 0.06f, " ", Color::Magenta << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Magenta << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Magenta << 4, false },
	{ 0.06f, " ", Color::White << 4, false },
	{ 0.06f, " ", Color::Magenta << 4, false }
};

static const Actor::ActorData enemyInitActorData[static_cast<int>(EnemyType::Count)] =
{
	{  },
	{  },
	{  },
	{  },
	{ "S", {}, Color::Magenta, 11 }
};

static const Enemy::EnemyData enemyInitEnemyData[static_cast<int>(EnemyType::Count)] =
{
	{  },
	{  },
	{  },
	{  },
	{ 5, 1, 1, 2.1f, 1.0f, 3.5f, { occurAnimSeqSegfault, _countof(occurAnimSeqSegfault) }}
};

GameLevel::GameLevel(const Vector2<int>& mapSize)
	: mapSize(mapSize)
{
	Initialize();
}

void GameLevel::Initialize()
{
	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	dangerGrid.assign(gridSize.y, std::vector<int>(gridSize.x, 0));
	wallGrid.assign(gridSize.y, std::vector<bool>(gridSize.x, false));
	quadTree = std::make_unique<QuadTree>();

	std::unique_ptr<PlayerController> newPlayerController = std::make_unique<PlayerController>();
	playerController = newPlayerController.get();
	AddNewActor(std::move(newPlayerController));

	systemCorePool = std::make_unique<ObjectPool<SystemCore>>();
	compilerTurretPool = std::make_unique<ObjectPool<CompilerTurret>>();
	debuggerNodePool = std::make_unique<ObjectPool<DebuggerNode>>();
	garbageCollectorPool = std::make_unique<ObjectPool<GarbageCollector>>();
	mutexBarrierPool = std::make_unique<ObjectPool<MutexBarrier>>();
	exceptionHandlerPool = std::make_unique<ObjectPool<ExceptionHandler>>();
	segfaultPool = std::make_unique<ObjectPool<Segfault>>();
}

void GameLevel::Tick(float deltaTime)
{
	if (Input::Instance().GetKeyDown(VK_ESCAPE))
	{
		Game::Instance().ToggleMenu();
		return;
	}

	if (Input::Instance().GetMouseButtonDown(1))
	{
		// Spawn Segfault
		{
			const int typeIdx = static_cast<int>(EnemyType::Segfault);
			std::unique_ptr<Enemy> newEnemy = segfaultPool->Acquire();
			Actor::ActorData actorData{ enemyInitActorData[typeIdx] };
			actorData.position = Input::Instance().MouseWorldPosition();
			newEnemy->As<Actor>()->Initialize(actorData);
			newEnemy->Initialize(enemyInitEnemyData[typeIdx]);
			AddNewActor(std::move(newEnemy));
		}
		return;
	}

	survivalTime += deltaTime;
	lastDeltaTime = deltaTime;

	Super::Tick(deltaTime);
	PhysicsManager::Instance().UpdatePhysics(Super::GetActors(), deltaTime);
}

void GameLevel::Draw()
{
	Super::Draw();

	quadTree->Draw();

	DrawHUD();
}

bool GameLevel::BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos)
{
	Vector2<int> screenPos;
	if (!Renderer::Instance().TransformWorldToScreen(groundPos, screenPos))
	{
		return false;
	}

	const int typeIdx = static_cast<int>(type);
	const Vector2<int> pos{ groundPos };

	// Acquire and Add Tower to Level
	std::unique_ptr<Tower> tower;
	switch (type)
	{
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

	// Update DangerGrid for Navigation
	UpdateDangerGrid(type, pos, 1);

	// Insert Tower to QuadTree
	quadTree->Insert(towerActor);

	return true;
}

void GameLevel::UpdateDangerGrid(const TowerType& type, const Vector2<int>& pos, const int value)
{
	wallGrid[pos.y][pos.x] = true;
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
}

std::vector<Actor*> GameLevel::QueryActorsInRange(const Bounds& bounds)
{
	return quadTree->Query(bounds);
}

void GameLevel::RemoveActorInQuadTree(Actor* actor)
{
	quadTree->Remove(actor);
}

const Tower::TowerData& GameLevel::GetTowerInitData(const TowerType& type) const
{
	return towerInitTowerData[static_cast<int>(type)];
}

void GameLevel::DrawHUD()
{
	// time
	sprintf_s(bufferStime, "%02d:%02d", static_cast<int>(survivalTime / 60), static_cast<int>(survivalTime) % 60);
	Renderer::Instance().Submit(bufferStime, Vector2<int>(1, 2), Color::Gray);

	// Build
	TowerType selectedTowerTypeToBuild = playerController->GetSelectedTowerTypeToBuild();
	Renderer::Instance().Submit("Build: ", Vector2<int>(1, 4), Color::Gray);
	Renderer::Instance().Submit("[1]Compiler", Vector2<int>(8, 4), (selectedTowerTypeToBuild == TowerType::CompilerTurret ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[2]Debugger", Vector2<int>(20, 4), (selectedTowerTypeToBuild == TowerType::DebuggerNode ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[3]GC", Vector2<int>(32, 4), (selectedTowerTypeToBuild == TowerType::GarbageCollector ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[4]Mutex", Vector2<int>(38, 4), (selectedTowerTypeToBuild == TowerType::MutexBarrier ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[5]Except", Vector2<int>(47, 4), (selectedTowerTypeToBuild == TowerType::ExceptionHandler ? Color::Green : Color::Gray));
	Renderer::Instance().Submit("[R]Cancel", Vector2<int>(57, 4), Color::Gray);

	// Other
	Renderer::Instance().Submit("Action: [Space]JumpToSystemCore", Vector2<int>(1, 5), Color::Gray);

	// camPos
	const Vector2<int> viewTransform = Renderer::Instance().GetViewTransform() * -1;
	sprintf_s(bufferCamPos, "Cam:(%d, %d)", viewTransform.x, viewTransform.y);
	Renderer::Instance().Submit(bufferCamPos, Vector2<int>(1, 7), Color::Gray);

	// mousePos
	const Vector2<int> mousePosition{ Input::Instance().MouseWorldPosition() };
	sprintf_s(bufferMousePos, "Cursor:(%d, %d)", mousePosition.x, mousePosition.y);
	Renderer::Instance().Submit(bufferMousePos, Vector2<int>(1, 8), Color::Gray);

	// fps
	sprintf_s(bufferFPS, "FPS:%d", static_cast<int>(1.0f / lastDeltaTime));
	Renderer::Instance().Submit(bufferFPS, Vector2<int>(1, 9), Color::DarkGray);

	DrawBorderLine();
}

void GameLevel::DrawBorderLine()
{
	static const int screenX = Engine::Instance().GetScreenSize().x;
	static const int screenY = Engine::Instance().GetScreenSize().y;

	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 0), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 1), Color::Gray);
	Renderer::Instance().Submit("Terminal Defense: Bug Siege", Vector2<int>(1, 1), Color::Cyan);
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
