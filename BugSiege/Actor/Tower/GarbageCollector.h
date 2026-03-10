#pragma once

#include "Actor/Tower/Tower.h"

class GarbageCollector : public Tower
{
	RTTI_DECLARATIONS(GarbageCollector, Tower)

private:
	virtual void TickAttack(float deltaTime) override;

private:
	virtual void Attack() override;
	virtual void UpdateGridsForNavigation() override;
};
