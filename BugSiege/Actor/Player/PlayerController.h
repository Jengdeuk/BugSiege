#pragma once

#include "Actor/Actor.h"
#include "Camera/Camera.h"

#include <memory>

using namespace JD;

class PlayerController : public Actor
{
	RTTI_DECLARATIONS(PlayerController, Actor)

public:
	virtual void BeginPlay() override;
	virtual void Tick(float timeDelta) override;

private:
	void InputCameraMovement(float deltaTime);

private:
	std::unique_ptr<Camera> camera;
};

