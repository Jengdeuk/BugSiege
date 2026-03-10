#pragma once

#include "Actor/Actor.h"

using namespace JD;

class ExplosionEffect : public Actor
{
	RTTI_DECLARATIONS(ExplosionEffect, Actor)

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
};
