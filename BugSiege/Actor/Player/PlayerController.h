#pragma once

#include "Actor/Actor.h"
#include "Camera/Camera.h"
#include "Common/Enums.h"

#include <memory>

using namespace JD;

class PlayerController : public Actor
{
	RTTI_DECLARATIONS(PlayerController, Actor)

public:
	virtual void BeginPlay() override;
	virtual void Tick(float timeDelta) override;
	virtual void Draw() override;

public:
	inline const TowerType& GetSelectedTowerTypeToBuild() const { return selectedTowerTypeToBuild; }

private:
	void ProcessInput(float deltaTime);

private:
	void InputSelectTowerTypeToBuild();
	void InputSelectGroundToBuild();

private:
	void InputCameraMovement(float deltaTime);

private:
	TowerType selectedTowerTypeToBuild = TowerType::Count;

private:
	std::unique_ptr<Camera> camera;
};

