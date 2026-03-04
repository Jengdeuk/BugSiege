#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Tower : public Actor
{
	RTTI_DECLARATIONS(Tower, Actor)

	struct TowerData
	{
		float reloadTime = 0.0f;
		float radius = 0.0f;
		int damage = 0;
	};

public:


private:
	TowerData towerData;
};
