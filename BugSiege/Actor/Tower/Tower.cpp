#include "Tower.h"
#include "Level/GameLevel.h"
#include "Physics/Collision.h"

void Tower::Initialize(const TowerData& initData)
{
	SetCollisionFilter({ TOWER, ENEMY });
	towerData = initData;

	hasBuilt = false;
	hasCollapsed = false;
	curState = State::Count;
}

void Tower::BeginPlay()
{
	Super::BeginPlay();
	ChangeState(State::Build);
}

void Tower::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	switch (curState)
	{
	case State::Build:
		TickBuild(deltaTime);
		break;
	case State::Idle:
		TickIdle(deltaTime);
		break;
	case State::Collapsed:
		TickCollapsed(deltaTime);
		break;
	default:
		break;
	}
}

void Tower::ChangeState(const State nxtState)
{
	switch (nxtState)
	{
	case State::Build:
		PlayAnimation(towerData.buildAnimSeq);
		break;
	case State::Collapsed:
		PlayAnimation(towerData.collapsedAnimSeq);
		break;
	default:
		break;
	}

	curState = nxtState;
}

void Tower::TickBuild(float deltaTime)
{
	if (GetAnimSequence().seq)
	{
		return;
	}

	hasBuilt = true;
	ChangeState(State::Idle);
}

void Tower::TickCollapsed(float deltaTime)
{
	if (GetAnimSequence().seq)
	{
		return;
	}

	Destroy();
}

void Tower::Damaged(const int damage)
{
	towerData.health -= damage;
	if (towerData.health <= 0)
	{
		hasCollapsed = true;
		UpdateGridForNavigation();
		GetOwner()->As<GameLevel>()->RemoveActorInQuadTree(this);
		ChangeState(State::Collapsed);
	}
}
