#pragma once
#include "SDLWrapper.hpp"

struct SpriteData
{
	std::string name = "";
	int width = 0, height = 0, cols = 0, rows = 0;

	void Load(std::string dir, int sprWidth, int sprHeight, int sprCols, int sprRows);
	void Draw(int col, int row, gobl::vec2i pos, gobl::vec2i scale);
};

class Card
{
public:
	static const gobl::vec2i CARD_RECT;
	std::string name;
	SpriteData& spriteData;
	int sprIndex;
	uint8_t type;

	bool mouseOver(gobl::vec2<int> pos);
	void Draw(gobl::vec2<int> pos);

	Card(const std::string& name, SpriteData& spriteData, const int& sprIndex, const uint8_t& t)
		: name(name), spriteData(spriteData), sprIndex(sprIndex), type(t)
	{
	}
};