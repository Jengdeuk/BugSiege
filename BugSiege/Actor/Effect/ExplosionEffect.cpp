#include "ExplosionEffect.h"

static const Actor::AnimFrame animSeqRGB[] =
{
	{ 0.06f, "#", Color::Red, Color::Black },
	{ 0.06f, "#", Color::White, Color::Black },
	{ 0.06f, "#", Color::Green, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black },
	{ 0.06f, "*", Color::Blue, Color::Black },
	{ 0.06f, "*", Color::White, Color::Black }
};

void ExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	PlayAnimation(ANIMSEQ(animSeqRGB));
}

void ExplosionEffect::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (GetAnimSequence().seq)
	{
		return;
	}

	Destroy();
}
