#pragma once

#include "Common/RTTI.h"

#include "Math/Color.h"
#include "Math/Vector2.h"

#include <memory>

namespace JD
{
	class Level;

	class JD_API Actor : public RTTI
	{
		RTTI_DECLARATIONS(Actor, RTTI)

		struct InitData
		{
			const char* image = nullptr;
			Vector2<float> position;
			Color color = Color::Gray;
			int sortingOrder = 0;
		};

		struct CollisionFilter
		{
			unsigned int layer = 0;
			unsigned int mask = 0;
		};

		inline bool ShouldCollide(const CollisionFilter& a, const CollisionFilter& b)
		{
			return (a.mask & b.layer) && (b.mask & a.layer);
		}

	public:
		Actor() = default;
		Actor(const InitData& initData);
		virtual ~Actor();

		Actor(const Actor&) = delete;
		Actor& operator=(const Actor&) = delete;

	public:
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:
		virtual void TransformUpdate(float deltaTime);

	public:
		bool TransformWorldToScreen(Vector2<int>& outScreenPos);

	public:
		void Destroy();

	public:
		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && destroyRequested == false; }
		inline bool DestroyRequested() const { return destroyRequested; }

	public:
		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		inline void SetPosition(const Vector2<float>& newPosition) { position = newPosition; }
		inline const Vector2<float>& GetPosition() const { return position; }

		inline void SetCollisionFilter(const CollisionFilter& filter) { collisionFilter = filter; }
		inline const CollisionFilter& GetCollisionFilter() const { return collisionFilter; }

		void SetImage(const char* newImage);
		void SetImage(std::unique_ptr<char[]> newImage);
		inline void SetColor(const Color newColor) { color = newColor; }

		inline const InitData& GetInitData() const { return initData; }

	private:
		bool hasBeganPlay = false;
		bool isActive = true;
		bool destroyRequested = false;
		Level* owner = nullptr;

	private:
		std::unique_ptr<char[]> image = nullptr;
		Vector2<float> position{};
		Color color = Color::White;
		int sortingOrder = 0;

	private:
		InitData initData;
		CollisionFilter collisionFilter;
	};
}
