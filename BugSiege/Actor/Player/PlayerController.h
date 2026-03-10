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
	inline void SetSelectedTowerTypeToBuild() { selectedTowerTypeToBuild = TowerType::Count; }
	inline void SetCamPositionToStart() { camera->SetPositionToStart(); }
	inline void SetCamPositionBack() { camera->SetPositionBack(); }
	void ShakeCam(const float shakeStrength);

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

