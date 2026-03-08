#pragma once

#include "Math/Vector2.h"

#include <vector>
#include <memory>

namespace JD
{
	class Actor;
}

using namespace JD;

class PhysicsManager
{
private:
	PhysicsManager() = default;

	PhysicsManager(const PhysicsManager&) = delete;
	PhysicsManager& operator=(const PhysicsManager&) = delete;

public:
	static PhysicsManager& Instance();

public:
	using Actors = std::vector<std::unique_ptr<Actor>>;
	void UpdatePhysics(const Actors& actors, float deltaTime);

private:
	void ResolvePenetration(const Actors& actors, float deltaTime);

private:
	bool CheckPenetration(float& outP, const Vector2<float>& aPos, const Vector2<float>& bPos, const float colRadius);
};
