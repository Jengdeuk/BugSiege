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
		backColor = actorData.backColor;
		sortingOrder = actorData.sortingOrder;
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
		animSequence = {};
		colorAnimSequence = {};
		backColorAnimSequence = {};
	}

	void Actor::Tick(float deltaTime)
	{
		TickAnim(deltaTime);
		if (animSequence.seq)
		{
			return;
		}

		TickColorAnim(deltaTime);
		TickBackColorAnim(deltaTime);
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

		Renderer::Instance().Submit(image, screenPos, color | (backColor << 4), sortingOrder);
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
		SetImage(animSequence.seq[idx].image);
		SetColor(animSequence.seq[idx].color);
		SetBackColor(animSequence.seq[idx].backColor);
	}

	void Actor::PlayColorAnimation(const AnimSequence& animSequence)
	{
		this->colorAnimSequence = animSequence;
		curColorAnimIdx = 0;
		JumpColorAnimFrame(curColorAnimIdx);
	}

	void Actor::JumpColorAnimFrame(const int idx)
	{
		colorAnimTimer.Reset();
		colorAnimTimer.SetTargetTime(colorAnimSequence.seq[idx].playTime);
		SetColor(colorAnimSequence.seq[idx].color);
	}

	void Actor::PlayBackColorAnimation(const AnimSequence& animSequence)
	{
		this->backColorAnimSequence = animSequence;
		curBackColorAnimIdx = 0;
		JumpBackColorAnimFrame(curBackColorAnimIdx);
	}

	void Actor::JumpBackColorAnimFrame(const int idx)
	{
		backColorAnimTimer.Reset();
		backColorAnimTimer.SetTargetTime(backColorAnimSequence.seq[idx].playTime);
		SetBackColor(backColorAnimSequence.seq[idx].backColor);
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
			SetBackColor(actorData.backColor);
			return;
		}

		JumpAnimFrame(++curAnimIdx);
	}

	void Actor::TickColorAnim(float deltaTime)
	{
		if (!colorAnimSequence.seq)
		{
			return;
		}

		colorAnimTimer.Tick(deltaTime);
		if (!colorAnimTimer.IsTimeOut())
		{
			return;
		}

		if (curColorAnimIdx == colorAnimSequence.size - 1)
		{
			colorAnimSequence.seq = nullptr;
			SetColor(actorData.color);
			return;
		}

		JumpColorAnimFrame(++curColorAnimIdx);
	}

	void Actor::TickBackColorAnim(float deltaTime)
	{
		if (!backColorAnimSequence.seq)
		{
			return;
		}

		backColorAnimTimer.Tick(deltaTime);
		if (!backColorAnimTimer.IsTimeOut())
		{
			return;
		}

		if (curBackColorAnimIdx == backColorAnimSequence.size - 1)
		{
			backColorAnimSequence.seq = nullptr;
			SetBackColor(actorData.backColor);
			return;
		}

		JumpBackColorAnimFrame(++curBackColorAnimIdx);
	}
}
