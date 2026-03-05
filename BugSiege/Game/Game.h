#pragma once

#include "Common/Common.h"

#include <memory>
#include <vector>

namespace JD
{
	class Level;
}

using namespace JD;

class Game
{
public:
	enum class State
	{
		None = -1,
		GamePlay = 0,
		Menu = 1,
		Length
	};

private:
	Game();
	~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

public:
	static Game& Instance();

public:
	void Run();

public:
	void ToggleMenu();
	void QuitEngine();

private:
	std::vector<std::unique_ptr<Level>> levels;
	State state = State::GamePlay;
};
