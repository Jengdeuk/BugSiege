#include "ScreenBuffer.h"
#include "Renderer.h"

namespace JD
{
	ScreenBuffer::ScreenBuffer(const Vector2<int>& screenSize)
		: screenSize(screenSize)
	{
		buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

		if (buffer == INVALID_HANDLE_VALUE)
		{
			MessageBoxA(nullptr, "ScreenBuffer - Failed to create buffer.", "Buffer creation error", MB_OK);
			DEBUG_BREAK("Failed to create buffer.");
		}

		SMALL_RECT rect{ 0, 0, static_cast<short>(screenSize.x - 1), static_cast<short>(screenSize.y - 1) };
		if (!SetConsoleWindowInfo(buffer, true, &rect))
		{
			DEBUG_BREAK("Failed to set console window info.");
		}

		if (!SetConsoleScreenBufferSize(buffer, screenSize))
		{
			DEBUG_BREAK("Failed to set console buffer size.");
		}

		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(buffer, &info);
		info.bVisible = false;
		SetConsoleCursorInfo(buffer, &info);
	}

	ScreenBuffer::~ScreenBuffer()
	{
		if (buffer)
		{
			CloseHandle(buffer);
		}
	}

	void ScreenBuffer::Clear()
	{
		DWORD writtenCount = 0;
		FillConsoleOutputCharacterA(buffer, ' ', screenSize.x * screenSize.y, Vector2<int>(0, 0), &writtenCount);
	}

	void ScreenBuffer::Draw(CHAR_INFO* charInfo)
	{
		SMALL_RECT writeRegion{ 0, 0, static_cast<short>(screenSize.x - 1), static_cast<short>(screenSize.y - 1) };
		WriteConsoleOutputA(buffer, charInfo, screenSize, Vector2<int>(0, 0), &writeRegion);
	}
}
