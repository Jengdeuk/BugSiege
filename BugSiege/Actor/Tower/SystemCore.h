#pragma once

#include "Actor/Tower/Tower.h"

using namespace JD;

class SystemCore : public Tower
{
	RTTI_DECLARATIONS(SystemCore, Tower)

private:
	virtual void UpdateDangerGrid() override;
};
