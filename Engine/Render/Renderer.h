#pragma once

#include "Math/Color.h"
#include "Math/Vector2.h"

#include <memory>
#include <vector>

namespace JD
{
	class ScreenBuffer;

	class JD_API Renderer
	{
		struct Frame
		{
			Frame(int bufferCount);

			void Clear(const Vector2<int>& screenSize);

			std::unique_ptr<CHAR_INFO[]> charInfoArray = nullptr;
			std::unique_ptr<int[]> sortingOrderArray = nullptr;
		};

		struct RenderCommand
		{
			const char* text = nullptr;
			Vector2<int> position;
			Color color = Color::Gray;
			int sortingOrder = 0;
		};

	private:
		Renderer();
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	public:
		static Renderer& Instance();
		void Init(const Vector2<int>& screenSize);

	public:
		void Draw();
		void Submit(const char* text, const Vector2<int>& position, Color color = Color::White, int sortingOrder = 0);

	public:
		inline void SetViewTransform(const Vector2<int>& transform) { viewTransform = transform; }
		inline const Vector2<int>& GetViewTransform() const { return viewTransform; }

	private:
		void Clear();
		void Present(); // 활성화 버퍼 교환
		inline ScreenBuffer* GetCurrentBuffer() { return screenBuffers[currentBufferIndex].get(); }

	private:
		Vector2<int> viewTransform;

	private:
		bool initialized = false;
		Vector2<int> screenSize{};
		std::unique_ptr<Frame> frame = nullptr;
		std::unique_ptr<ScreenBuffer> screenBuffers[2] = {};
		int currentBufferIndex = 0;
		std::vector<RenderCommand> renderQueue;
	};
}
