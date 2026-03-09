#pragma once

#include "Common/RTTI.h"

#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Util/Timer.h"

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
			WORD color = 0;
			WORD backColor = 0;
			int sortingOrder = 0;
		};

		struct AnimFrame
		{
			float playTime = 0.0f;
			const char* image = nullptr;
			WORD color = 0;
			WORD backColor = 0;
		};

		struct AnimSequence
		{
			const AnimFrame* seq = nullptr;
			int size = 0;
		};

		struct CollisionFilter
		{
			unsigned int layer = 0;
			unsigned int mask = 0;
		};

	public:
		void Initialize(const ActorData& initData);
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:
		inline bool ShouldCollide(Actor* other)
		{
			return (collisionFilter.mask & other->collisionFilter.layer) && (other->collisionFilter.mask & collisionFilter.layer);
		}

	public:
		bool TransformWorldToScreen(Vector2<int>& outScreenPos);
		bool TransformWorldToScreen(const Vector2<float>& worldPosf, Vector2<int>& outScreenPos);

	public:
		void PlayAnimation(const AnimSequence& animSequence);
		void JumpAnimFrame(const int idx);
		void PlayColorAnimation(const AnimSequence& animSequence);
		void JumpColorAnimFrame(const int idx);
		void PlayBackColorAnimation(const AnimSequence& animSequence);
		void JumpBackColorAnimFrame(const int idx);

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

		inline const AnimSequence& GetAnimSequence() const { return animSequence; }

		inline void SetImage(const char* newImage) { image = newImage; }
		inline void SetColor(const WORD newColor) { color = newColor; }
		inline void SetBackColor(const WORD newBackColor) { backColor = newBackColor; }

	private:
		void TickAnim(float deltaTime);
		void TickColorAnim(float deltaTime);
		void TickBackColorAnim(float deltaTime);

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
		AnimSequence animSequence;
		Timer animTimer;
		int curAnimIdx = 0;
		AnimSequence colorAnimSequence;
		Timer colorAnimTimer;
		int curColorAnimIdx = 0;
		AnimSequence backColorAnimSequence;
		Timer backColorAnimTimer;
		int curBackColorAnimIdx = 0;

	private:
		const char* image = nullptr;
		Vector2<float> position{};
		WORD color = 0;
		WORD backColor = 0;
		int sortingOrder = 0;
	};
}
