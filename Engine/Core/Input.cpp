#include "Input.h"
#include "Util/Util.h"
#include "Engine/Engine.h"

#include <Windows.h>
#include <cassert>

namespace JD
{
	Input& Input::Instance()
	{
		static Input instance;
		return instance;
	}

	bool Input::GetKeyDown(int keyCode)
	{
		return keyState[keyCode].isKeyDown && keyState[keyCode].wasKeyDown == false;
	}

	bool Input::GetKeyUp(int keyCode)
	{
		return keyState[keyCode].isKeyDown == false && keyState[keyCode].wasKeyDown;
	}

	bool Input::GetKey(int keyCode)
	{
		return keyState[keyCode].isKeyDown && keyState[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButtonDown(int buttonCode)
	{
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyState[keyCode].isKeyDown && !keyState[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButtonUp(int buttonCode)
	{
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return !keyState[keyCode].isKeyDown && keyState[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButton(int buttonCode)
	{
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyState[keyCode].isKeyDown;
	}

	void Input::ProcessInput()
	{
		static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;

			DWORD mode;
			GetConsoleMode(inputHandle, &mode);
			mode |= ENABLE_MOUSE_INPUT;
			mode |= ENABLE_EXTENDED_FLAGS;
			mode &= ~ENABLE_QUICK_EDIT_MODE;
			SetConsoleMode(inputHandle, mode);
		}

		const int recordCount = 256;
		INPUT_RECORD records[recordCount] = {};

		DWORD eventReadCount = 0;
		if (PeekConsoleInput(inputHandle, records, recordCount, &eventReadCount) && eventReadCount > 0)
		{
			if (ReadConsoleInput(inputHandle, records, recordCount, &eventReadCount))
			{
				for (int ix = 0; ix < static_cast<int>(eventReadCount); ++ix)
				{
					INPUT_RECORD& record = records[ix];
					switch (record.EventType)
					{
					case KEY_EVENT:
					{
						if (record.Event.KeyEvent.bKeyDown)
						{
							keyState[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = true;
						}
						else
						{
							keyState[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = false;
						}
					}
					break;
					case MOUSE_EVENT:
					{
						mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;
						mousePosition.y = record.Event.MouseEvent.dwMousePosition.Y;

						//mousePosition.x = Util::Clamp<int>(mousePosition.x, 0, Engine::Instance().GetMapSize().x - 1);
						//mousePosition.y = Util::Clamp<int>(mousePosition.y, 0, Engine::Instance().GetMapSize().y - 1);

						keyState[VK_LBUTTON].isKeyDown = (record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
						keyState[VK_RBUTTON].isKeyDown = (record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
					}
					break;
					}
				}
			}
		}
	}

	void Input::SavePreviousInputStates()
	{
		for (int i = 0; i < 255; ++i)
		{
			keyState[i].wasKeyDown = keyState[i].isKeyDown;
		}
	}
}
