#include "Segfault.h"

#include "Render/Renderer.h"
#include "Navigation/AStar.h"

Segfault::Segfault()
	: navigation(std::make_unique<AStar>())
{
}

void Segfault::Draw()
{
	Super::Draw();

	for (const Vector2<int> pos : path)
	{
		Vector2<int> screenPos;
		if (!TransformWorldToScreen(Vector2<float>(pos), screenPos))
		{
			return;
		}
		Renderer::Instance().Submit(".", screenPos, WORD(Color::Red), 5);
	}
}

void Segfault::UpdatePath(const Vector2<int>& goalPos, const std::vector<std::vector<bool>>& wallGrid, const std::vector<std::vector<int>>& dangerGrid)
{
	path = navigation->FindPath(Vector2<int>(GetPosition()), goalPos, wallGrid, dangerGrid);
}
