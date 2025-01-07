#pragma once
#include "card.hpp"

class Suspect : public Card
{
private:
	std::vector<std::string> motives{};
	int motiveIndex = 0;
public:
	bool isKiller = false;
	bool foundMotive = false;

	Suspect(const std::string& name, SpriteData& spriteData, const int& sprIndex, std::vector<std::string> motives);

	std::string GetMotive();

	void DrawMini(gobl::vec2<float> pos, int scale = 1U);
};

