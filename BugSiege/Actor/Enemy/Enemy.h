#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Tower;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

	struct EnemyData
	{
		int tier = 0;
		int health = 0;
		int damage = 0;
		float radius = 0;
		float reloadTime = 0.0f;
		float speed = 0.0f;
		Actor::AnimSequence occurAnimSeq;
		Actor::AnimSequence attackAnimSeq;
		Actor::AnimSequence damagedAnimSeq;
		Actor::AnimSequence fixedAnimSeq;
	};
	
	enum class State
	{
		Occur,
		Search,
		Trace,
		Attack,
		Fixed,
		Count
	};

public:
	void Initialize(const EnemyData& initData);
	virtual void BeginPlay();
	virtual void Tick(float deltaTime) override;

public:
	void ChangeState(const State nxtState);

protected:
	virtual void TickOccur(float deltaTime);
	virtual void TickSearch(float deltaTime);
	virtual void TickTrace(float deltaTime);
	virtual void TickAttack(float deltaTime);
	virtual void TickFixed(float deltaTime);

public:
	virtual void Attack();
	virtual void Damaged(const int damage);

public:
	inline bool HasOccured() const { return hasOccured; }
	inline bool HasFixed() const { return hasFixed; }
	inline const EnemyData& GetEnemyData() const { return enemyData; }

private:
	virtual bool CanAttack();

private:
	EnemyData enemyData;

private:
	bool hasOccured = false;
	bool hasFixed = false;
	State curState = State::Count;

private:
	Timer attackTimer;

protected:
	Tower* target = nullptr;
};
