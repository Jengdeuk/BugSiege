#pragma once

#include "Actor/Tower/Tower.h"

class CompilerTurret : public Tower
{
	RTTI_DECLARATIONS(CompilerTurret, Tower)

private:
	virtual void Attack() override;
	virtual void UpdateGridsForNavigation() override;
};
