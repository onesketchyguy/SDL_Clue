#include "card.hpp"

void SpriteData::Draw(int col, int row, gobl::vec2<int> pos, uint32_t scale)
{
	SDLWrapper::DrawSprite(name, 
		gobl::vec2<float>{ static_cast<float>(pos.x), static_cast<float>(pos.y) },
		gobl::vec2<int>{ static_cast<int>(scale), static_cast<int>(scale) },
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

void Card::Draw(gobl::vec2<int> pos, uint32_t scale)
{
	int col = sprIndex % spriteData.cols;
	int row = sprIndex / spriteData.cols;
	spriteData.Draw(col, row, pos, scale);
}