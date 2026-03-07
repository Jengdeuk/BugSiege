#include "PlayerController.h"

#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Render/Renderer.h"

#include "Level/GameLevel.h"
#include "Actor/Tower/Tower.h"

static const char* imgs[5] =
{
	"C", "D", "G", "M", "E"
};

void PlayerController::BeginPlay()
{
	Super::BeginPlay();

	camera = std::make_unique<Camera>(100.0f);
}

void PlayerController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	ProcessInput(deltaTime);
}

void PlayerController::Draw()
{
	static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();
	static const Vector2<int> mapSize = Engine::Instance().GetMapSize();

	const Vector2<int> mousePos = Input::Instance().MousePosition();

	// culling
	if (mousePos.x <= mapStartPos.x || mousePos.x >= mapStartPos.x + mapSize.x - 1
		|| mousePos.y <= mapStartPos.y || mousePos.y >= mapStartPos.y + mapSize.y - 1)
	{
		return;
	}

	const int typeIdx = static_cast<int>(selectedTowerTypeToBuild);
	Renderer::Instance().Submit(imgs[typeIdx], mousePos, WORD(Color::Green) | WORD(Color::DarkBlue) << 4, 15);

	const int cx = mousePos.x;
	const int cy = mousePos.y;
	const int r = static_cast<int>(GetOwner()->As<GameLevel>()->GetTowerInitData(selectedTowerTypeToBuild).radius);
	for (int y = cy - r; y <= cy + r; ++y)
	{
		for (int x = cx - r; x <= cx + r; ++x)
		{
			if (x == cx && y == cy)
			{
				continue;
			}

			// culling
			if (x <= mapStartPos.x || x >= mapStartPos.x + mapSize.x - 1 || y <= mapStartPos.y || y >= mapStartPos.y + mapSize.y - 1)
			{
				continue;
			}

			int dx = x - cx;
			int dy = y - cy;
			if (dx * dx + dy * dy <= r * r)
			{
				Renderer::Instance().Submit(" ", { x, y }, WORD(Color::Blue) << 4, 10);
			}
		}
	}
}

void PlayerController::ProcessInput(float deltaTime)
{
	InputSelectTowerTypeToBuild();
	InputSelectGroundToBuild();
	InputCameraMovement(deltaTime);
}

void PlayerController::InputSelectTowerTypeToBuild()
{
	if (Input::Instance().GetKeyDown('R'))
	{
		selectedTowerTypeToBuild = TowerType::Count;
	}
	else if (Input::Instance().GetKeyDown('1'))
	{
		selectedTowerTypeToBuild = TowerType::CompilerTurret;
	}
	else if (Input::Instance().GetKeyDown('2'))
	{
		selectedTowerTypeToBuild = TowerType::DebuggerNode;
	}
	else if (Input::Instance().GetKeyDown('3'))
	{
		selectedTowerTypeToBuild = TowerType::GarbageCollector;
	}
	else if (Input::Instance().GetKeyDown('4'))
	{
		selectedTowerTypeToBuild = TowerType::MutexBarrier;
	}
	else if (Input::Instance().GetKeyDown('5'))
	{
		selectedTowerTypeToBuild = TowerType::ExceptionHandler;
	}
}

void PlayerController::InputSelectGroundToBuild()
{
	if (selectedTowerTypeToBuild == TowerType::Count)
	{
		return;
	}

	if (Input::Instance().GetMouseButtonDown(0))
	{
		if (GetOwner()->As<GameLevel>()->BuildTowerToGround(selectedTowerTypeToBuild, Input::Instance().MouseWorldPosition()))
		{
			selectedTowerTypeToBuild = TowerType::Count;
		}
		return;
	}
}

void PlayerController::InputCameraMovement(float deltaTime)
{
	if (Input::Instance().GetKey(VK_SPACE))
	{
		camera->SetPositionToStart();
		return;
	}

	static const Vector2<int> mapSize = Engine::Instance().GetMapSize();

	const Vector2<int> mousePos = Input::Instance().MousePosition();

	Vector2<float> moveDirection;

	bool isMove = false;
	if (Input::Instance().GetKey('D') || mousePos.x >= mapSize.x - 1)
	{
		isMove = true;
		moveDirection.x = 1;
	}
	if (Input::Instance().GetKey('A') || mousePos.x <= 0)
	{
		isMove = true;
		moveDirection.x = -1;
	}
	if (Input::Instance().GetKey('W') || mousePos.y <= 9)
	{
		isMove = true;
		moveDirection.y = 1;
	}
	if (Input::Instance().GetKey('S') || mousePos.y >= mapSize.y + 9)
	{
		isMove = true;
		moveDirection.y = -1;
	}

	if (isMove)
	{
		camera->Move(moveDirection.Normalized(), deltaTime);
	}
}
