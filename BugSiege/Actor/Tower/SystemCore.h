#pragma once

#include "Actor/Tower/Tower.h"

class SystemCore : public Tower
{
	RTTI_DECLARATIONS(SystemCore, Tower)

private:
	virtual void TickAttack(float deltaTime) override {}

private:
	virtual void Damaged(const int damage) override;

private:
	virtual void UpdateGridsForNavigation() override;
};
