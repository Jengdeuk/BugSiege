#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace JD
{
	class JD_API Input
	{
		friend class Engine;

		struct KeyState
		{
			bool isKeyDown = false;
			bool wasKeyDown = false;
		};

	public:
		static Input& Instance();

	private:
		Input() = default;
		~Input() = default;

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

	public:
		bool GetKeyDown(int keyCode);
		bool GetKeyUp(int keyCode);
		bool GetKey(int keyCode);

	public:
		bool GetMouseButtonDown(int buttonCode);
		bool GetMouseButtonUp(int buttonCode);
		bool GetMouseButton(int buttonCode);
		Vector2<float> MouseWorldPosition();
		inline const Vector2<int>& MousePosition() const { return mousePosition; }

	private:
		void ProcessInput();
		void SavePreviousInputStates();

	private:
		Vector2<int> mousePosition;
		KeyState keyState[255] = {};
	};
}
