#include "GameLevel.h"

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Util/Util.h"

#include "Game/Game.h"
#include "Actor/Player/PlayerController.h"
#include "Actor/Tower/Tower.h"

#include <memory>

using namespace JD;

GameLevel::GameLevel(const Vector2<int>& mapSize)
	: mapSize(mapSize)
{
	std::unique_ptr<PlayerController> newPlayerController = std::make_unique<PlayerController>();
	AddNewActor(std::move(newPlayerController));

	// Spawn Tower
	Actor::InitData initData;
	initData.image = "@";
	initData.color = Color::Cyan;
	initData.sortingOrder = 15;
	initData.position = Vector2<float>(127.0f, 127.0f);
	AddNewActor(std::make_unique<Tower>(initData));
	initData.position = Vector2<float>(127.0f, 128.0f);
	AddNewActor(std::make_unique<Tower>(initData));
	initData.position = Vector2<float>(128.0f, 127.0f);
	AddNewActor(std::make_unique<Tower>(initData));
	initData.position = Vector2<float>(128.0f, 128.0f);
	AddNewActor(std::make_unique<Tower>(initData));

	initData.image = "@";
	initData.color = Color::Red;
	initData.position = Vector2<float>(0.0f, 0.0f);
	initData.sortingOrder = 15;
	AddNewActor(std::make_unique<Tower>(initData));

	initData.image = "@";
	initData.color = Color::Red;
	initData.position = Vector2<float>(255.0f, 0.0f);
	initData.sortingOrder = 15;
	AddNewActor(std::make_unique<Tower>(initData));

	initData.image = "@";
	initData.color = Color::Red;
	initData.position = Vector2<float>(0.0f, 255.0f);
	initData.sortingOrder = 15;
	AddNewActor(std::make_unique<Tower>(initData));

	initData.image = "@";
	initData.color = Color::Red;
	initData.position = Vector2<float>(255.0f, 255.0f);
	initData.sortingOrder = 15;
	AddNewActor(std::make_unique<Tower>(initData));
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

void GameLevel::DrawHUD()
{
	static const int screenX = Engine::Instance().GetScreenSize().x;
	static const int screenY = Engine::Instance().GetScreenSize().y;

	Renderer::Instance().Submit("+--------------------------------------------------------------+", Vector2<int>(0, 0), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 1), Color::Gray);
	Renderer::Instance().Submit("Terminal Defense: Bug Siege", Vector2<int>(1, 1), Color::Cyan);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 1), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 2), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 2), Color::Gray);
	Renderer::Instance().Submit("+--------------------------------------------------------------+", Vector2<int>(0, 3), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 4), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 4), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 5), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 5), Color::Gray);
	Renderer::Instance().Submit("+--------------------------------------------------------------+", Vector2<int>(0, 6), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 7), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 7), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(0, 8), Color::Gray);
	Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, 8), Color::Gray);
	Renderer::Instance().Submit("+--------------------------------------------------------------+", Vector2<int>(0, 9), Color::Gray);
	for (int i = 10; i < screenY - 1; ++i)
	{
		Renderer::Instance().Submit("|", Vector2<int>(0, i), Color::Gray);
		Renderer::Instance().Submit("|", Vector2<int>(screenX - 1, i), Color::Gray);
	}
	Renderer::Instance().Submit("+--------------------------------------------------------------+", Vector2<int>(0, screenY - 1), Color::Gray);

	// time
	sprintf_s(buffer_stime, "%02d:%02d", static_cast<int>(survivalTime / 60), static_cast<int>(survivalTime) % 60);
	Renderer::Instance().Submit(buffer_stime, Vector2<int>(1, 2), Color::Gray);

	// camPos
	const Vector2<int> viewTransform = Renderer::Instance().GetViewTransform() * -1;
	sprintf_s(buffer_camPos, "Cam:(%d, %d)", viewTransform.x, viewTransform.y);
	Renderer::Instance().Submit(buffer_camPos, Vector2<int>(1, 4), Color::Gray);

	// fps
	sprintf_s(buffer_fps, "FPS:%d", static_cast<int>(1.0f / lastDeltaTime));
	Renderer::Instance().Submit(buffer_fps, Vector2<int>(1, 5), Color::DarkGray);
}
