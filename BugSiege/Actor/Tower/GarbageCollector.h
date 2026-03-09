#pragma once

#include "Actor/Tower/Tower.h"

class GarbageCollector : public Tower
{
	RTTI_DECLARATIONS(GarbageCollector, Tower)

private:
	virtual void UpdateGridForNavigation() override;
};
