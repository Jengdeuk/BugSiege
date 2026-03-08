#include "Actor.h"

#include "Render/Renderer.h"

#include "Engine/Engine.h"

namespace JD
{
	void Actor::Initialize(const ActorData& initData)
	{
		actorData = initData;

		hasBeganPlay = false;
		destroyRequested = false;

		image = actorData.image;
		position = actorData.position;
		color = actorData.color;
		sortingOrder = actorData.sortingOrder;
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
		animSequence = {};
	}

	void Actor::Tick(float deltaTime)
	{
		TickAnim(deltaTime);
	}

	void Actor::Draw()
	{
		if (destroyRequested || !image)
		{
			return;
		}

		Vector2<int> screenPos;
		if (!TransformWorldToScreen(screenPos))
		{
			return;
		}

		Renderer::Instance().Submit(image, screenPos, color, sortingOrder);
	}

	bool Actor::TransformWorldToScreen(Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		Vector2<int> worldPos = Vector2<int>(position);

		// 1. world space -> view space
		Vector2<int> viewPos = worldPos + Renderer::Instance().GetViewTransform();

		// 2. veiw space -> screen space
		outScreenPos = viewPos;
		outScreenPos.y *= -1;
		outScreenPos += mapHalfSize;
		outScreenPos += mapStartPos;

		// 3. culling
		if (outScreenPos.x <= mapStartPos.x || outScreenPos.x >= mapStartPos.x + mapSize.x - 1
			|| outScreenPos.y <= mapStartPos.y || outScreenPos.y >= mapStartPos.y + mapSize.y - 1)
		{
			return false;
		}

		return true;
	}

	bool Actor::TransformWorldToScreen(const Vector2<float>& worldPosf, Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		Vector2<int> worldPos = Vector2<int>(worldPosf);

		// 1. world space -> view space
		Vector2<int> viewPos = worldPos + Renderer::Instance().GetViewTransform();

		// 2. veiw space -> screen space
		outScreenPos = viewPos;
		outScreenPos.y *= -1;
		outScreenPos += mapHalfSize;
		outScreenPos += mapStartPos;

		// 3. culling
		if (outScreenPos.x <= mapStartPos.x || outScreenPos.x >= mapStartPos.x + mapSize.x - 1
			|| outScreenPos.y <= mapStartPos.y || outScreenPos.y >= mapStartPos.y + mapSize.y - 1)
		{
			return false;
		}

		return true;
	}

	void Actor::PlayAnimation(const AnimSequence& animSequence)
	{
		this->animSequence = animSequence;
		curAnimIdx = 0;
		JumpAnimFrame(curAnimIdx);
	}

	void Actor::JumpAnimFrame(const int idx)
	{
		animTimer.Reset();
		animTimer.SetTargetTime(animSequence.seq[idx].playTime);
		if (!animSequence.seq[idx].isChangeColorOnly)
		{
			SetImage(animSequence.seq[idx].image);
		}
		SetColor(animSequence.seq[idx].color);
	}

	void Actor::Destroy()
	{
		destroyRequested = true;
	}

	void Actor::TickAnim(float deltaTime)
	{
		if (!animSequence.seq)
		{
			return;
		}

		animTimer.Tick(deltaTime);
		if (!animTimer.IsTimeOut())
		{
			return;
		}

		if (curAnimIdx == animSequence.size - 1)
		{
			animSequence.seq = nullptr;
			SetImage(actorData.image);
			SetColor(actorData.color);
			return;
		}

		JumpAnimFrame(++curAnimIdx);
	}
}
