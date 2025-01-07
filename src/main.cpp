#include "SDLWrapper.hpp"
#include <SDL2/SDL_main.h>
#include "game.hpp"

#undef main // FIXME: Don't do this

int main(int argc, char* argv[])
{
	SDLWrapper* wrapper = new SDLWrapper{ "Clue", 800, 800};

	Clue demo{};
	while (true)
	{
		if (!SDLWrapper::Update()) break;
		demo.OnUserUpdate(SDLWrapper::deltaTime());
	}

	delete wrapper;

	return 0;
}