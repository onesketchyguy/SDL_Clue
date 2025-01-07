#include "button.hpp"

void Button::Draw()
{
	gobl::vec2<int> size = gobl::vec2<int>{ static_cast<int>(text.size()) * 8, 12 };
	SDLWrapper::DrawRect(pos.x, pos.y, size.x, size.y, sdl::BLACK);

	if (SDLWrapper::getMouse().y > pos.y && SDLWrapper::getMouse().y < pos.y + size.y && SDLWrapper::getMouse().x > pos.x && SDLWrapper::getMouse().x < pos.x + size.x)
	{
		if (SDLWrapper::getMouse().bDown(0)) onClick();

		SDLWrapper::OutlineRect(pos.x, pos.y, size.x, size.y, sdl::WHITE);
	} else SDLWrapper::OutlineRect(pos.x, pos.y, size.x, size.y, sdl::GREY);

	SDLWrapper::DrawString(text, { pos.x + 2, pos.y + 2 }, sdl::WHITE);
}
