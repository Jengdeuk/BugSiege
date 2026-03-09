#pragma once

#include "Actor/Tower/Tower.h"

class MutexBarrier : public Tower
{
	RTTI_DECLARATIONS(MutexBarrier, Tower)

private:
	virtual void UpdateGridForNavigation() override;
};
