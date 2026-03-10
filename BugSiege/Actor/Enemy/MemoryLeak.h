#pragma once

#include "Enemy.h"

class MemoryLeak : public Enemy
{
	RTTI_DECLARATIONS(MemoryLeak, Enemy)

private:
	virtual void Damaged(const int damage) override;
};
