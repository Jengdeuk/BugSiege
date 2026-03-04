#include "PlayerController.h"

#include "Core/Input.h"
#include "Engine/Engine.h"

void PlayerController::BeginPlay()
{
	Super::BeginPlay();

	camera = std::make_unique<Camera>(100.0f);
}

void PlayerController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	InputCameraMovement(deltaTime);
}

void PlayerController::InputCameraMovement(float deltaTime)
{
	if (Input::Instance().GetKey(VK_SPACE))
	{
		camera->SetPositionToStart();
		return;
	}

	static const int mapX = Engine::Instance().GetMapSize().x;
	static const int mapY = Engine::Instance().GetMapSize().y;

	const int mouseX = Input::Instance().MousePosition().x;
	const int mouseY = Input::Instance().MousePosition().y;

	Vector2<float> moveDirection;

	bool isMove = false;
	if (Input::Instance().GetKey('D') || mouseX >= mapX - 1)
	{
		isMove = true;
		moveDirection.x = 1;
	}
	if (Input::Instance().GetKey('A') || mouseX <= 0)
	{
		isMove = true;
		moveDirection.x = -1;
	}
	if (Input::Instance().GetKey('W') || mouseY <= 9)
	{
		isMove = true;
		moveDirection.y = 1;
	}
	if (Input::Instance().GetKey('S') || mouseY >= mapY + 9)
	{
		isMove = true;
		moveDirection.y = -1;
	}

	if (isMove)
	{
		camera->Move(moveDirection.Normalized(), deltaTime);
	}
}
