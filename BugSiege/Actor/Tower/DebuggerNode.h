#pragma once

#include "Actor/Tower/Tower.h"

using namespace JD;

class DebuggerNode : public Tower
{
	RTTI_DECLARATIONS(DebuggerNode, Tower)

private:
	virtual void UpdateDangerGrid() override;
};
