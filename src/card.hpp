#pragma once
#include "SDLWrapper.hpp"

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
	void DrawZoomed(gobl::vec2<int> pos);

	Card(const std::string& name, SpriteData& spriteData, const int& sprIndex, const uint8_t& t)
		: name(name), spriteData(spriteData), sprIndex(sprIndex), type(t)
	{
	}
};