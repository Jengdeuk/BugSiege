#pragma once

#include "Actor/Tower/Tower.h"

class ExceptionHandler : public Tower
{
	RTTI_DECLARATIONS(ExceptionHandler, Tower)

private:
	virtual void UpdateGridForNavigation() override;
};
