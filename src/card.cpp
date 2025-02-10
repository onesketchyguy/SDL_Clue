#include "card.hpp"

const gobl::vec2i Card::CARD_RECT = { 80, 120 };

void SpriteData::Draw(int col, int row, gobl::vec2i pos, gobl::vec2i scale)
{
	SDLWrapper::DrawSprite(name, pos, scale,
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
	return SDLWrapper::getMouse().over(pos, CARD_RECT);
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
	int size = static_cast<int>(name.size()) * (fontSize >> 1);
	while (size >= CARD_RECT.x)
	{
		fontSize--;
		size = static_cast<int>(name.size()) * (fontSize >> 1);
	}

	SDLWrapper::DrawString(name, pos + gobl::vec2i{ 4, CARD_RECT.y - 20 }, sdl::BLACK, fontSize);
}

void Card::DrawZoomed(gobl::vec2<int> pos)
{
	gobl::vec2i bigRect = { CARD_RECT.x * 3, CARD_RECT.y * 3 };
	SDLWrapper::DrawRect(pos.x, pos.y, bigRect.x, bigRect.y, sdl::GREY);
	SDLWrapper::OutlineRect(pos.x, pos.y, bigRect.x, bigRect.y, sdl::BLACK);
	SDLWrapper::OutlineRect(pos.x + 1, pos.y + 1, bigRect.x - 2, bigRect.y - 2, sdl::DARK_GREY);

	int col = sprIndex % spriteData.cols;
	int row = sprIndex / spriteData.cols;

	SDLWrapper::DrawRect(pos.x + 10, pos.y + 10, bigRect.x - 20, bigRect.x - 20, sdl::LIGHT_GREY);
	spriteData.Draw(col, row, pos + gobl::vec2i{ 10, 10 }, { bigRect.x - 20, bigRect.x - 20 });

	int fontSize = 16;
	int size = static_cast<int>(name.size()) * (fontSize >> 1);
	while (size >= bigRect.x)
	{
		fontSize--;
		size = static_cast<int>(name.size()) * (fontSize >> 1);
	}

	SDLWrapper::DrawString(name, pos + gobl::vec2i{ 4, bigRect.y - 20 }, sdl::BLACK, fontSize);
}