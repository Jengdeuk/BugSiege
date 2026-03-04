#include "Game.h"

#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"

#include <memory>

using namespace JD;

Game::Game()
{
	levels.emplace_back(std::make_unique<GameLevel>(Engine::Instance().GetMapSize()));
	levels.emplace_back(std::make_unique<MenuLevel>());

	state = State::GamePlay;

	Engine::Instance().SetNewLevel(levels[0].get());
}

Game::~Game()
{
}

Game& Game::Instance()
{
	static Game instance;
	return instance;
}

void Game::Run()
{
	Engine::Instance().Run();
}

void Game::ToggleMenu()
{
	system("cls");

	state = static_cast<State>(1 - static_cast<int>(state));
	Engine::Instance().SetNewLevel(levels[static_cast<int>(state)].get());
}

void Game::QuitEngine()
{
	Engine::Instance().QuitEngine();
}
