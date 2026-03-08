#pragma once

#include "Actor/Tower/Tower.h"

using namespace JD;

class CompilerTurret : public Tower
{
	RTTI_DECLARATIONS(CompilerTurret, Tower)

private:
	virtual void UpdateDangerGrid() override;
};
