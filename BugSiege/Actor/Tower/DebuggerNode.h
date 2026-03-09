#pragma once

#include "Actor/Tower/Tower.h"

class DebuggerNode : public Tower
{
	RTTI_DECLARATIONS(DebuggerNode, Tower)

private:
	virtual void UpdateGridsForNavigation() override;
};
