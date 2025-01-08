#include "card.hpp"

const gobl::vec2i Card::CARD_RECT = { 80, 120 };

void SpriteData::Draw(int col, int row, gobl::vec2i pos, gobl::vec2i scale)
{
	SDLWrapper::DrawSprite(name,
		gobl::vec2<float>{ static_cast<float>(pos.x), static_cast<float>(pos.y) }, scale,
		gobl::vec2<int>{ col* width, row* height },
		gobl::vec2<int>{ width, height }
	);
}

void SpriteData::Load(std::string dir, int sprWidth, int sprHeight, int sprCols, int sprRows)
{
	this->name = dir;
	this->cols = sprCols;
	this->rows = sprRows;
	this->height = sprHeight;
	this->width = sprWidth;

	SDLWrapper::LoadSprite(dir);
}

bool Card::mouseOver(gobl::vec2<int> pos)
{
	return SDLWrapper::getMouse().x < pos.x + CARD_RECT.x && SDLWrapper::getMouse().x > pos.x &&
		SDLWrapper::getMouse().y < pos.y + CARD_RECT.y && SDLWrapper::getMouse().y > pos.y;
}

void Card::Draw(gobl::vec2<int> pos)
{
	SDLWrapper::DrawRect(pos.x, pos.y, CARD_RECT.x, CARD_RECT.y, sdl::GREY);
	SDLWrapper::OutlineRect(pos.x, pos.y, CARD_RECT.x, CARD_RECT.y, sdl::BLACK);
	SDLWrapper::OutlineRect(pos.x + 1, pos.y + 1, CARD_RECT.x - 2, CARD_RECT.y - 2, sdl::DARK_GREY);

	int col = sprIndex % spriteData.cols;
	int row = sprIndex / spriteData.cols;

	SDLWrapper::DrawRect(pos.x + 10, pos.y + 10, CARD_RECT.x - 20, CARD_RECT.x - 20, sdl::LIGHT_GREY);
	spriteData.Draw(col, row, pos + gobl::vec2i{ 10, 10 }, { CARD_RECT.x - 20, CARD_RECT.x - 20 });

	int fontSize = 16;
	int size = name.size() * (fontSize >> 1);
	while (size >= CARD_RECT.x)
	{
		fontSize--;
		size = name.size() * (fontSize >> 1);
	}

	SDLWrapper::DrawString(name, pos + gobl::vec2i{ 4, CARD_RECT.y - 20 }, sdl::BLACK, fontSize);
}