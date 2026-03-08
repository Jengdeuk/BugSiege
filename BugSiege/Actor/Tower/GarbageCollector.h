#pragma once

#include "Actor/Tower/Tower.h"

using namespace JD;

class GarbageCollector : public Tower
{
	RTTI_DECLARATIONS(GarbageCollector, Tower)

private:
	virtual void UpdateDangerGrid() override;
};
