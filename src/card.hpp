#pragma once
#include "SDLWrapper.hpp"

struct SpriteData
{
	std::string name = "";
	int width = 0, height = 0, cols = 0, rows = 0;

	void Load(std::string dir, int sprWidth, int sprHeight, int sprCols, int sprRows);
	void Draw(int col, int row, gobl::vec2<int> pos, uint32_t scale);
};

class Card
{
public:
	std::string name;
	SpriteData& spriteData;
	int sprIndex;

	void Draw(gobl::vec2<int> pos, uint32_t scale = 0U);

	Card(const std::string& name, SpriteData& spriteData, const int& sprIndex)
		: name(name), spriteData(spriteData), sprIndex(sprIndex)
	{
	}
};