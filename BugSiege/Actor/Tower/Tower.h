#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Tower : public Actor
{
	RTTI_DECLARATIONS(Tower, Actor)
		
public:
	Tower(const Actor::InitData& actorInitData);
	virtual ~Tower();
};
