#include "GameLevel.h"

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Util/Util.h"

#include "Game/Game.h"

#include "Actor/ObjectPool.h"
#include "Actor/Player/PlayerController.h"

#include "Actor/Tower/SystemCore.h"
#include "Actor/Tower/CompilerTurret.h"

#include "Actor/Enemy/Segfault.h"

#include <memory>

using namespace JD;

static const Actor::ActorData towerInitActorData[static_cast<int>(TowerType::Count)] =
{
	{ "C", {}, Color::Green, 10 },
};

static const Tower::TowerData towerInitTowerData[static_cast<int>(TowerType::Count)] =
{
	{ 1, 3.0f, 1.0f },
};

static const Actor::ActorData enemyInitActorData[static_cast<int>(EnemyType::Count)] =
{
	{  },
	{  },
	{  },
	{  },
	{ "S", {}, Color::Magenta, 10 }
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
	std::unique_ptr<PlayerController> newPlayerController = std::make_unique<PlayerController>();
	AddNewActor(std::move(newPlayerController));

	systemCorePool = std::make_unique<ObjectPool<SystemCore>>();
	compilerTurretPool = std::make_unique<ObjectPool<CompilerTurret>>();

	// Spawn SystemCore
	{
		Actor::ActorData actorData;
		actorData.image = "@";
		actorData.color = Color::Cyan;
		actorData.sortingOrder = 15;

		actorData.position = Vector2<float>(127.0f, 127.0f);
		std::unique_ptr<SystemCore> systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(127.0f, 128.0f);
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(128.0f, 127.0f);
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));

		actorData.position = Vector2<float>(128.0f, 128.0f);
		systemCore = systemCorePool->Acquire();
		systemCore->As<Actor>()->Initialize(actorData);
		AddNewActor(std::move(systemCore));
	}

	segfaultPool = std::make_unique<ObjectPool<Segfault>>();
	{
		std::unique_ptr<Enemy> enemy = segfaultPool->Acquire();

		const int typeIdx = static_cast<int>(EnemyType::Segfault);
		Actor::ActorData actorData{ enemyInitActorData[typeIdx] };
		actorData.position = { 130.0f, 130.0f };
		enemy->As<Actor>()->Initialize(actorData);
		enemy->Initialize(enemyInitEnemyData[typeIdx]);

		AddNewActor(std::move(enemy));
	}
}

void GameLevel::Tick(float deltaTime)
{
	if (Input::Instance().GetKeyDown(VK_ESCAPE))
	{
		Game::Instance().ToggleMenu();
		return;
	}

	Super::Tick(deltaTime);

	survivalTime += deltaTime;
	lastDeltaTime = deltaTime;
}

void GameLevel::Draw()
{
	Super::Draw();

	DrawHUD();
}

bool GameLevel::BuildTowerToGround(const TowerType& type, const Vector2<float>& groundPos)
{
	std::unique_ptr<Tower> tower;

	const int typeIdx = static_cast<int>(type);
	switch (type)
	{
	case TowerType::CompilerTurret:
		tower = compilerTurretPool->Acquire();
		break;
	default:
		return false;
	}

	Actor::ActorData actorData{ towerInitActorData[typeIdx] };
	actorData.position = groundPos;
	tower->As<Actor>()->Initialize(actorData);
	tower->Initialize(towerInitTowerData[typeIdx]);

	AddNewActor(std::move(tower));

	return true;
}

void GameLevel::DrawHUD()
{
	// time
	sprintf_s(buffer_stime, "%02d:%02d", static_cast<int>(survivalTime / 60), static_cast<int>(survivalTime) % 60);
	Renderer::Instance().Submit(buffer_stime, Vector2<int>(1, 2), Color::Gray);

	// camPos
	const Vector2<int> viewTransform = Renderer::Instance().GetViewTransform() * -1;
	sprintf_s(buffer_camPos, "Cam:(%d, %d)", viewTransform.x, viewTransform.y);
	Renderer::Instance().Submit(buffer_camPos, Vector2<int>(1, 4), Color::Gray);

	// camPos
	const Vector2<int> mousePosition{ Input::Instance().MouseWorldPosition() };
	sprintf_s(buffer_mousePos, "Mouse:(%d, %d)", mousePosition.x, mousePosition.y);
	Renderer::Instance().Submit(buffer_mousePos, Vector2<int>(1, 5), Color::Gray);

	// fps
	sprintf_s(buffer_fps, "FPS:%d", static_cast<int>(1.0f / lastDeltaTime));
	Renderer::Instance().Submit(buffer_fps, Vector2<int>(1, 8), Color::DarkGray);

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
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, 9), Color::Gray);
	for (int i = 10; i < screenY - 1; ++i)
	{
		Renderer::Instance().Submit("|", Vector2<int>(0, i), Color::Gray);
		Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, i), Color::Gray);
	}
	Renderer::Instance().Submit("+------------------------------------------------------------------------------------------------------+", Vector2<int>(0, screenY - 1), Color::Gray);
}
