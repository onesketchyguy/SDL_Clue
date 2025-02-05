#pragma once
#include "SDLWrapper.hpp"
#include "card.hpp"

class QuestionObject
{
public:
	std::string text;
	Card* card = nullptr;
	std::string answer;
	gobl::vec2i pos;
	gobl::vec2i rect;
	void Draw();
	bool mouseOver();
};