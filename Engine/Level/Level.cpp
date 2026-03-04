#include "Level.h"
#include "Actor/Actor.h"

namespace JD
{
	Level::Level()
	{
	}

	Level::~Level()
	{
	}

	Level::Level(Level&&) noexcept = default;

	Level& Level::operator=(Level&&) noexcept = default;

	void Level::BeginPlay()
	{
		for (std::unique_ptr<Actor>& actor : actors)
		{
			if (actor->HasBeganPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		for (std::unique_ptr<Actor>& actor : actors)
		{
			actor->Tick(deltaTime);
		}
	}

	void Level::Draw()
	{
		for (std::unique_ptr<Actor>& actor : actors)
		{
			actor->Draw();
		}
	}

	void Level::PhysicsUpdate(float deltaTime)
	{
	}

	void Level::TransformUpdate(float deltaTime)
	{
		for (std::unique_ptr<Actor>& actor : actors)
		{
			actor->TransformUpdate(deltaTime);
		}
	}

	void Level::AddNewActor(std::unique_ptr<Actor> newActor)
	{
		addRequestedActors.emplace_back(std::move(newActor));
		addRequestedActors.back()->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		for (auto it = actors.begin(); it < actors.end();)
		{
			if ((*it)->DestroyRequested())
			{
				it = actors.erase(it);
			}
			else
			{
				++it;
			}
		}

		if (addRequestedActors.empty())
		{
			return;
		}

		for (std::unique_ptr<Actor>& actor : addRequestedActors)
		{
			actors.emplace_back(std::move(actor));
		}

		addRequestedActors.clear();
	}
}
