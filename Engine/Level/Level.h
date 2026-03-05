#pragma once

#include "Common/RTTI.h"
#include "Actor/Actor.h"

#include <memory>
#include <vector>

namespace JD
{
	class JD_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level() = default;
		Level(const Level&) = delete;
		Level& operator=(const Level&) = delete;

	public:
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:
		virtual void PhysicsUpdate(float deltaTime);
		virtual void TransformUpdate(float deltaTime);

	public:
		void AddNewActor(std::unique_ptr<Actor>&& newActor);
		void ProcessAddAndDestroyActors();

	protected:
		using Actors = std::vector<std::unique_ptr<Actor>>;
		const Actors& GetActors() const { return actors; }

	private:
		Actors actors;
		Actors addRequestedActors;
	};
}
