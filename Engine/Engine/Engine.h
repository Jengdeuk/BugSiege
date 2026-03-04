#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace JD
{
	class Level;

	class JD_API Engine
	{
		struct EngineSetting
		{
			float frameRate = 0.0f;
			int screenW = 0;
			int screenH = 0;
			int mapW = 0;
			int mapH = 0;
		};

	private:
		Engine();
		virtual ~Engine();

		Engine(Engine&) = delete;
		Engine& operator=(Engine&) = delete;

	public:
		static Engine& Instance();

	public:
		void Run();
		void QuitEngine();
		void SetNewLevel(Level* newLevel);
		inline const Vector2<int> GetScreenSize() const { return Vector2<int>(setting.screenW, setting.screenH); }
		inline const Vector2<int> GetMapSize() const { return Vector2<int>(setting.mapW, setting.mapH); }

	private:
		void BeginPlay();
		void Tick(float deltaTime);
		void Draw();

	private:
		void LoadSetting();
		void Shutdown();

	private:
		bool isQuit = false;
		Level* mainLevel = nullptr;
		EngineSetting setting{};
	};
}
