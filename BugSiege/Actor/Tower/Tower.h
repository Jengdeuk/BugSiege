#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Tower : public Actor
{
	RTTI_DECLARATIONS(Tower, Actor)

	struct TowerData
	{
		int tier = 0;
		int damage = 0;
		float radius = 0.0f;
		float reloadTime = 0.0f;
	};

public:
	void Initialize(const TowerData& initData);

private:
	TowerData towerData;
};
