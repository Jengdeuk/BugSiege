#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Engine/Engine.h"

namespace JD
{
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
		memset(charInfoArray.get(), 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = std::make_unique<int[]>(bufferCount);
		memset(sortingOrderArray.get(), 0, sizeof(int) * bufferCount);
	}

	void Renderer::Frame::Clear(const Vector2<int>& screenSize)
	{
		const int width = screenSize.x;
		const int height = screenSize.y;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int index = (y * width) + x;

				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				sortingOrderArray[index] = -1;
			}
		}
	}

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	Renderer& Renderer::Instance()
	{
		static Renderer instance;
		return instance;
	}

	void Renderer::Init(const Vector2<int>& inScreenSize)
	{
		if (initialized)
		{
			return;
		}

		initialized = true;
		screenSize = inScreenSize;

		const int bufferCount = screenSize.x * screenSize.y;
		frame = std::make_unique<Frame>(bufferCount);
		frame->Clear(screenSize);

		screenBuffers[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBuffers[1]->Clear();

		Present();
	}

	void Renderer::Draw()
	{
		Clear();

		for (const RenderCommand& command : renderQueue)
		{
			if (!command.text)
			{
				continue;
			}

			if (command.position.y < 0 || command.position.y >= screenSize.y)
			{
				continue;
			}

			const int length = static_cast<int>(strlen(command.text));
			if (length < 0)
			{
				continue;
			}

			const int startX = command.position.x;
			const int endX = command.position.x + length - 1;
			if (endX < 0 || startX >= screenSize.x)
			{
				continue;
			}

			const int visibleStart = (startX < 0 ? 0 : startX);
			const int visibleEnd = (endX >= screenSize.x ? screenSize.x - 1 : endX);
			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				const int index = (command.position.y * screenSize.x) + x;
				if (frame->sortingOrderArray[index] > command.sortingOrder)
				{
					continue;
				}

				frame->charInfoArray[index].Char.AsciiChar = command.text[x - startX];
				frame->charInfoArray[index].Attributes = command.color;

				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		GetCurrentBuffer()->Draw(frame->charInfoArray.get());

		Present();

		renderQueue.clear();
	}

	void Renderer::Submit(const char* text, const Vector2<int>& position, Color color, int sortingOrder)
	{
		renderQueue.emplace_back(RenderCommand{ text, position, static_cast<WORD>(color), sortingOrder });
	}

	void Renderer::Submit(const char* text, const Vector2<int>& position, WORD color, int sortingOrder)
	{
		renderQueue.emplace_back(RenderCommand{ text, position, color, sortingOrder });
	}

	bool Renderer::TransformWorldToScreen(const Vector2<float>& fworldPos, Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		Vector2<int> worldPos = Vector2<int>(fworldPos);

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

	bool Renderer::TransformWorldToScreen(const Vector2<int>& worldPos, Vector2<int>& outScreenPos)
	{
		static const Vector2<int> mapSize = Engine::Instance().GetMapSize();
		static const Vector2<int> mapHalfSize = mapSize / 2;
		static const Vector2<int> mapStartPos = Engine::Instance().GetMapStartPos();

		// 1. world space -> view space
		Vector2<int> viewPos = worldPos + GetViewTransform();

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

	void Renderer::Clear()
	{
		frame->Clear(screenSize);
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Present()
	{
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());
		currentBufferIndex = 1 - currentBufferIndex;
	}
}
