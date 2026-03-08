#include "Enemy.h"
#include "Physics/Collision.h"

void Enemy::Initialize(const EnemyData& initData)
{
	SetCollisionFilter({ ENEMY, TOWER | ENEMY });
	enemyData = initData;

	hasOccured = false;
	curState = State::Count;
}

void Enemy::BeginPlay()
{
	Super::BeginPlay();
	ChangeState(State::Occur);
}

void Enemy::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	switch (curState)
	{
	case State::Occur:
		TickOccur(deltaTime);
		break;
	case State::Search:
		TickSearch(deltaTime);
		break;
	case State::Trace:
		TickTrace(deltaTime);
		break;
	case State::Attack:
		TickAttack(deltaTime);
		break;
	default:
		break;
	}
}

void Enemy::ChangeState(const State nxtState)
{
	switch (nxtState)
	{
	case State::Occur:
		PlayAnimation(enemyData.occurAnimSeq);
		break;
	case State::Attack:
		attackTimer.Reset();
		attackTimer.SetTargetTime(enemyData.reloadTime);
		break;
	default:
		break;
	}

	curState = nxtState;
}

void Enemy::TickOccur(float deltaTime)
{
	if (GetAnimSequence().seq)
	{
		return;
	}

	hasOccured = true;
	ChangeState(State::Search);
}

void Enemy::TickAttack(float deltaTime)
{
	attackTimer.Tick(deltaTime);
	if (!attackTimer.IsTimeOut())
	{
		return;
	}

	attackTimer.Reset();
	Attack();
}
