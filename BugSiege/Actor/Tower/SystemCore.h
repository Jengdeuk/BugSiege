#pragma once

#include "Actor/Tower/Tower.h"

class SystemCore : public Tower
{
	RTTI_DECLARATIONS(SystemCore, Tower)

public:
	virtual void Damaged(const int damage) override;

private:
	virtual void UpdateGridForNavigation() override;
};
