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
			int mapSX = 0;
			int mapSY = 0;
			int mapW = 0;
			int mapH = 0;
			int gridW = 0;
			int gridH = 0;
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
		inline void SetTimeScale(float scale) { timeScale = scale; }
		inline float GetTimeScale() const { return timeScale; }

	public:
		inline const Vector2<int> GetScreenSize() const { return Vector2<int>(setting.screenW, setting.screenH); }
		inline const Vector2<int> GetMapStartPos() const { return Vector2<int>(setting.mapSX, setting.mapSY); }
		inline const Vector2<int> GetMapSize() const { return Vector2<int>(setting.mapW, setting.mapH); }
		inline const Vector2<int> GetGridSize() const { return Vector2<int>(setting.gridW, setting.gridH); }

	private:
		void BeginPlay();
		void Tick(float deltaTime);
		void Draw();

	private:
		void LoadSetting();

	private:
		float timeScale = 0.0f;
		bool isQuit = false;
		Level* mainLevel = nullptr;
		EngineSetting setting{};
	};
}
