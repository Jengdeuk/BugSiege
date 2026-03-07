#pragma once

#include "Common/RTTI.h"

#include "Math/Color.h"
#include "Math/Vector2.h"

#include <memory>

class QuadNode;

namespace JD
{
	class Level;
	class IObjectPool;

	class JD_API Actor : public RTTI
	{
		RTTI_DECLARATIONS(Actor, RTTI)

		struct ActorData
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
		void Initialize(const ActorData& initData);
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:
		virtual void TransformUpdate(float deltaTime);

	public:
		bool TransformWorldToScreen(Vector2<int>& outScreenPos);
		bool TransformWorldToScreen(const Vector2<float>& worldPosf, Vector2<int>& outScreenPos);

	public:
		void Destroy();

	public:
		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool DestroyRequested() const { return destroyRequested; }

	public:
		inline const ActorData& GetActorData() const { return actorData; }

		inline void SetCollisionFilter(const CollisionFilter& filter) { collisionFilter = filter; }
		inline const CollisionFilter& GetCollisionFilter() const { return collisionFilter; }

		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		inline void SetOwnerPool(IObjectPool* newOwnerPool) { ownerPool = newOwnerPool; }
		inline IObjectPool* GetOwnerPool() const { return ownerPool; }

		inline void SetOwnerQuadNode(QuadNode* newOwnerQuadNode) { ownerQuadNode = newOwnerQuadNode; }
		inline QuadNode* GetOwnerQuadNode() const { return ownerQuadNode; }

		inline void SetPosition(const Vector2<float>& newPosition) { position = newPosition; }
		inline const Vector2<float>& GetPosition() const { return position; }

		void SetImage(const char* newImage);
		void SetImage(std::unique_ptr<char[]> newImage);
		inline void SetColor(const Color newColor) { color = newColor; }

	private:
		ActorData actorData;
		CollisionFilter collisionFilter;

	private:
		bool hasBeganPlay = false;
		bool destroyRequested = false;
		Level* owner = nullptr;
		IObjectPool* ownerPool = nullptr;
		QuadNode* ownerQuadNode = nullptr;

	private:
		std::unique_ptr<char[]> image = nullptr;
		Vector2<float> position{};
		Color color = Color::White;
		int sortingOrder = 0;
	};
}
