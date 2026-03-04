#pragma once

#include "Common/RTTI.h"

#include <memory>
#include <vector>

namespace JD
{
	class Actor;

	class JD_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		Level(const Level&) = delete;
		Level& operator=(const Level&) = delete;

		Level(Level&&) noexcept;
		Level& operator=(Level&&) noexcept;

	public:
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:
		virtual void PhysicsUpdate(float deltaTime);
		virtual void TransformUpdate(float deltaTime);

	public:
		void AddNewActor(std::unique_ptr<Actor> newActor);
		void ProcessAddAndDestroyActors();

	protected:
		using Actors = std::vector<std::unique_ptr<Actor>>;
		const Actors& GetActors() const { return actors; }

	private:
		Actors actors;
		Actors addRequestedActors;
	};
}
