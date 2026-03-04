#pragma once

#include "Level/Level.h"
#include "Math/Color.h"

#include <memory>
#include <vector>

using namespace JD;

class MenuLevel : public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level)

	struct MenuItem
	{
		using OnSelected = void (*)();

		MenuItem(const char* text, OnSelected onSelected)
			: onSelected(onSelected)
		{
			size_t length = strlen(text) + 1;
			this->text = std::make_unique<char[]>(length);
			std::memcpy(this->text.get(), text, length);
		}

		std::unique_ptr<char[]> text;
		OnSelected onSelected = nullptr;
	};

public:
	MenuLevel();
	~MenuLevel();

public:
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	void LoadTitle(const char* filename);

private:
	int currentIndex = 0;
	Color selectedColor = Color::Yellow;
	Color unselectedColor = Color::Gray;
	std::vector<std::unique_ptr<MenuItem>> items;

private:
	std::unique_ptr<char[]> titleImg;
	std::vector<std::unique_ptr<char[]>> titleImgSliced;
};
