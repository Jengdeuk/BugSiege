#include "MemoryLeak.h"

#include "Level/GameLevel.h"

void MemoryLeak::Damaged(const int damage)
{
	Super::Damaged(damage);

	if (HasFixed())
	{
		GetOwner()->As<GameLevel>()->SummonMemoryLeakSmalls(GetPosition());
	}
}
