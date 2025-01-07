#include "portal.h"

#include "SDLWrapper.hpp"

void sdl::CardRect(gobl::vec2<int> pos, gobl::vec2<int> size, SDL_Color col)
{
	SDLWrapper::DrawRect(pos.x, pos.y, size.x, size.y, col);
	SDLWrapper::OutlineRect(pos.x, pos.y, size.x, size.y, { 0, 0, 0, 255U });
}