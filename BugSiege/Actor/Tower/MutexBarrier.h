#pragma once

#include "Actor/Tower/Tower.h"

class MutexBarrier : public Tower
{
	RTTI_DECLARATIONS(MutexBarrier, Tower)

private:
	virtual void TickAttack(float deltaTime) override;

private:
	virtual void Attack() override;
	virtual void UpdateGridsForNavigation() override;
};
