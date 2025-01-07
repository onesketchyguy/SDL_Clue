#pragma once
#include "SDLWrapper.hpp"

class QuestionObject
{
public:
	std::string text;
	std::string answer;
	gobl::vec2<int> pos;
	void Draw();
	bool MouseOver();
};