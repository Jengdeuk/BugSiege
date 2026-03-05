#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

	struct EnemyData
	{
		int health = 0;
		float attackRadius = 0;
		float speed = 0.0f;
	};

public:
	void Initialize(const EnemyData& initData);

private:
	EnemyData enemyData;
};
