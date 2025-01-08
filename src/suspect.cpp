#include "suspect.hpp"
#include "SDLWrapper.hpp"

Suspect::Suspect(const std::string& name, SpriteData& spriteData, const int& sprIndex, std::vector<std::string> motives, const uint8_t& type)
	: Card(name, spriteData, sprIndex, type), motives(motives)
{
	motiveIndex = rand() % motives.size();
}

std::string Suspect::GetMotive()
{
	return motives.at(motiveIndex);
}

void Suspect::DrawMini(gobl::vec2<float> pos, int scale)
{
	int col = sprIndex % spriteData.cols;
	int row = sprIndex / spriteData.cols;

	SDLWrapper::DrawSprite(spriteData.name, gobl::vec2<float>{ pos.x - scale, pos.y - scale }, gobl::vec2<int>{ scale * 2, scale * 2 },
		gobl::vec2<int>{ col* spriteData.width, row* spriteData.height }, gobl::vec2<int>{ spriteData.width, spriteData.height });

	/*pge->DrawPartialSprite(pos,
		olc::vf2d{ (float)32.0f, (float)32.0f },
		spriteData.sprite,
		olc::vf2d{ float(col * spriteData.width), float(row * spriteData.height) },
		olc::vf2d{ float(spriteData.width), float(spriteData.height) }
	);*/
}