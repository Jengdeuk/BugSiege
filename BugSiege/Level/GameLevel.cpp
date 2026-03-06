#include "GameLevel.h"

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Util/Util.h"

#include "Game/Game.h"
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

static const Actor::ActorData towerInitActorData[static_cast<int>(TowerType::Count)] =
{
	{ "C", {}, Color::Green, 11 },
	{ "D", {}, Color::Green, 11 },
	{ "G", {}, Color::Green, 11 },
	{ "M", {}, Color::Green, 11 },
	{ "E", {}, Color::Green, 11 }
};

static const Tower::TowerData towerInitTowerData[static_cast<int>(TowerType::Count)] =
{
	{ 1, 1, 4.0f, 0.25f },
	{ 2, 1, 6.0f, 0.25f },
	{ 3, 3, 3.0f, 3.0f },
	{ 4, 1, 5.0f, 5.0f },
	{ 5, 1, 4.0f, 1.0f }
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
	{ 1, 0.5f, 1 }
};

GameLevel::GameLevel(const Vector2<int>& mapSize)
	: mapSize(mapSize)
{
	Initialize();
}

void GameLevel::Initialize()
{
	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	quadTree = std::make_unique<QuadTree>();

	std::unique_ptr<PlayerController> newPlayerController = std::make_unique<PlayerController>();
	AddNewActor(std::move(newPlayerController));

	systemCorePool = std::make_unique<ObjectPool<SystemCore>>();
	compilerTurretPool = std::make_unique<ObjectPool<CompilerTurret>>();
	debuggerNodePool = std::make_unique<ObjectPool<DebuggerNode>>();
	garbageCollectorPool = std::make_unique<ObjectPool<GarbageCollector>>();
	mutexBarrierPool = std::make_unique<ObjectPool<MutexBarrier>>();
	exceptionHandlerPool = std::make_unique<ObjectPool<ExceptionHandler>>();
	segfaultPool = std::make_unique<ObjectPool<Segfault>>();

	// Spawn SystemCore
	{
		Actor::ActorData actorData;
		actorData.image = "@";
		actorData.color = Color::Cyan;
		actorData.sortingOrder = 15;

		actorData.position = Vector2<float>(0.0f, 0.0f);
		std::unique_ptr<SystemCore> systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(0.0f, static_cast<float>(gridSize.y - 1));
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(static_cast<float>(gridSize.x - 1), 0.0f);
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(static_cast<float>(gridSize.x - 1), static_cast<float>(gridSize.y - 1));
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));
	}

	// Spawn Segfault
	{
		std::unique_ptr<Enemy> enemy = segfaultPool->Acquire();

		const int typeIdx = static_cast<int>(EnemyType::Segfault);
		Actor::ActorData actorData{ enemyInitActorData[typeIdx] };
		actorData.position = { 130.0f, 130.0f };
		enemy->As<Actor>()->Initialize(actorData);
		enemy->Initialize(enemyInitEnemyData[typeIdx]);

		segfault = enemy->As<Segfault>();

		AddNewActor(std::move(enemy));
	}

	dangerGrid.assign(gridSize.y, std::vector<int>(gridSize.x, 0));
	wallGrid.assign(gridSize.y, std::vector<bool>(gridSize.x, false));
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
		segfault->UpdatePath(Vector2<int>(Input::Instance().MouseWorldPosition()), wallGrid, dangerGrid);

		return;
	}

	Super::Tick(deltaTime);

	survivalTime += deltaTime;
	lastDeltaTime = deltaTime;
}

void GameLevel::Draw()
{
	Super::Draw();

	quadTree->Draw();

	DrawHUD();
}

bool GameLevel::BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos)
{
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
	wallGrid[pos.y][pos.x] = true;
	static const Vector2<int> gridSize = Engine::Instance().GetGridSize();
	const int r = static_cast<int>(towerInitTowerData[typeIdx].radius);
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
				++dangerGrid[y][x];
			}
		}
	}

	// Insert Tower to QuadTree
	quadTree->Insert(towerActor);

	return true;
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

	// camPos
	const Vector2<int> viewTransform = Renderer::Instance().GetViewTransform() * -1;
	sprintf_s(bufferCamPos, "Cam:(%d, %d)", viewTransform.x, viewTransform.y);
	Renderer::Instance().Submit(bufferCamPos, Vector2<int>(1, 4), Color::Gray);

	// mousePos
	const Vector2<int> mousePosition{ Input::Instance().MouseWorldPosition() };
	sprintf_s(bufferMousePos, "Mouse:(%d, %d)", mousePosition.x, mousePosition.y);
	Renderer::Instance().Submit(bufferMousePos, Vector2<int>(1, 5), Color::Gray);

	// fps
	sprintf_s(bufferFPS, "FPS:%d", static_cast<int>(1.0f / lastDeltaTime));
	Renderer::Instance().Submit(bufferFPS, Vector2<int>(1, 8), Color::DarkGray);

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
