#include "PhysicsManager.h"

#include "Collision.h"
#include "Actor/Actor.h"

PhysicsManager& PhysicsManager::Instance()
{
	static PhysicsManager instance;
	return instance;
}

void PhysicsManager::UpdatePhysics(const Actors& actors, float deltaTime)
{
	ResolvePenetration(actors, deltaTime);
}

void PhysicsManager::ResolvePenetration(const Actors& actors, float deltaTime)
{
	const int size = static_cast<int>(actors.size());
	for (int i = 0; i < size - 1; ++i)
	{
		Actor* a = actors[i].get();
		for (int j = i + 1; j < size; ++j)
		{
			Actor* b = actors[j].get();
			if (!a->ShouldCollide(b))
			{
				continue;
			} 

			float p = 0.0f;
			const Vector2<float>& aPos = a->GetPosition();
			const Vector2<float>& bPos = b->GetPosition();
			if (CheckPenetration(p, aPos, bPos, 0.5f))
			{
				const bool isAMovable = a->GetCollisionFilter().layer != CollisionBits::TOWER;
				const bool isBMovable = b->GetCollisionFilter().layer != CollisionBits::TOWER;

				const Vector2<float> n = bPos - aPos;
				if (isAMovable && isBMovable)
				{
					a->SetPosition(aPos + n * p * -0.5f);
					b->SetPosition(bPos + n * p * 0.5f);
				}
				else if (isAMovable)
				{
					a->SetPosition(aPos + n * p * -1.0f);
				}
				else if (isBMovable)
				{
					b->SetPosition(bPos + n * p);
				}
			}
		}
	}
}

bool PhysicsManager::CheckPenetration(float& outP, const Vector2<float>& aPos, const Vector2<float>& bPos, const float colRadius)
{
	const float lSq = LengthSq(aPos - bPos);
	const float rSq = 4 * colRadius * colRadius;

	outP = sqrt(rSq) - sqrt(lSq);

	return lSq <= rSq;
}