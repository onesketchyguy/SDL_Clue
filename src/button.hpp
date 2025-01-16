#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include "SDLWrapper.hpp"

class Button
{
public:
	std::function<void()> onClick;
	std::string text;
	gobl::vec2<int> pos;

	gobl::vec2i getEnd();
	void Draw();
};

#endif // BUTTON_H