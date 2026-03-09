#include "Enemy.h"

#include "Level/GameLevel.h"
#include "Physics/Collision.h"
#include "Actor/Tower/SystemCore.h"

void Enemy::Initialize(const EnemyData& initData)
{
	SetCollisionFilter({ ENEMY, TOWER | ENEMY });
	enemyData = initData;

	hasOccured = false;
	hasFixed = false;
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
	case State::Fixed:
		TickFixed(deltaTime);
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
	case State::Fixed:
		PlayAnimation(enemyData.fixedAnimSeq);
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

void Enemy::TickSearch(float deltaTime)
{
	ChangeState(State::Trace);
}

void Enemy::TickTrace(float deltaTime)
{
	// attack target
	if (CanAttack())
	{
		ChangeState(State::Attack);
		return;
	}

	const Vector2<float> pos{ GetPosition() };
	Vector2<float> nxtDst{ GetOwner()->As<GameLevel>()->GetNextNodeByFlowField(Vector2<int>(pos)) };
	const Vector2<float> dir{ (nxtDst - pos).Normalized() };
	SetPosition(pos + dir * GetEnemyData().speed * deltaTime);
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

void Enemy::TickFixed(float deltaTime)
{
	if (GetAnimSequence().seq)
	{
		return;
	}

	Destroy();
}

void Enemy::Attack()
{
	PlayBackColorAnimation(enemyData.attackAnimSeq);
	target->Damaged(GetEnemyData().damage);
}

void Enemy::Damaged(const int damage)
{
	PlayColorAnimation(enemyData.damagedAnimSeq);
	enemyData.health -= damage;
	if (enemyData.health <= 0)
	{
		hasFixed = true;
		// todo: update uniform grid
		ChangeState(State::Fixed);
	}
}

bool Enemy::CanAttack()
{
	const Vector2<float> pos = GetPosition();
	float nearDist = 12345678.9f;
	for (auto& systemCore : GetOwner()->As<GameLevel>()->GetSystemCores())
	{
		float dist = sqrt(LengthSq(systemCore->GetPosition() - pos));
		if (dist < nearDist)
		{
			nearDist = dist;
			target = systemCore;
		}
	}

	return nearDist <= GetEnemyData().radius;
}
