#include "button.hpp"

void Button::Draw()
{
	gobl::vec2<int> size = gobl::vec2<int>{ static_cast<int>(text.size()) * 8, 20 };
	bool mouseOver = (SDLWrapper::getMouse().y > pos.y && SDLWrapper::getMouse().y < pos.y + size.y && SDLWrapper::getMouse().x > pos.x && SDLWrapper::getMouse().x < pos.x + size.x);
	SDL_Color col = mouseOver ? sdl::GREY : sdl::WHITE;

	if (mouseOver && SDLWrapper::getMouse().bDown(0)) onClick();

	SDLWrapper::DrawRect(pos.x, pos.y, size.x, size.y, sdl::BLACK);
	SDLWrapper::OutlineRect(pos.x, pos.y, size.x, size.y, col);
	SDLWrapper::DrawString(text, { pos.x + 4, pos.y + 2 }, col);
}