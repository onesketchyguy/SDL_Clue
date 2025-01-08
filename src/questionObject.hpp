#pragma once
#include "SDLWrapper.hpp"

class QuestionObject
{
public:
	std::string text;
	std::string answer;
	gobl::vec2i pos;
	gobl::vec2i rect;
	void Draw();
	bool mouseOver();
};