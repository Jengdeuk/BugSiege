#pragma once

#include "Actor/Actor.h"

using namespace JD;

class Tower : public Actor
{
	RTTI_DECLARATIONS(Tower, Actor)

	struct TowerData
	{
		int tier = 0;
		int health = 0;
		int damage = 0;
		float radius = 0.0f;
		float reloadTime = 0.0f;
		Actor::AnimSequence buildAnimSeq;
		Actor::AnimSequence collapsedAnimSeq;
	};

	enum class State
	{
		Build,
		Idle,
		Collapsed,
		Count
	};

public:
	void Initialize(const TowerData& initData);
	virtual void BeginPlay();
	virtual void Tick(float deltaTime) override;

public:
	void ChangeState(const State nxtState);

protected:
	virtual void TickBuild(float deltaTime);
	virtual void TickIdle(float deltaTime) {}
	virtual void TickCollapsed(float deltaTime);

public:
	virtual void Damaged(const int damage);
	virtual void UpdateDangerGrid() = 0;

public:
	inline bool HasBuilt() const { return hasBuilt; }
	inline bool HasCollapsed() const { return hasCollapsed; }
	inline const TowerData& GetTowerData() const { return towerData; }

private:
	TowerData towerData;

private:
	bool hasBuilt = false;
	bool hasCollapsed = false;
	State curState = State::Count;
};
