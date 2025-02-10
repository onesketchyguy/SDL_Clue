#define SDL_MAIN_HANDLED
#include "SDLWrapper.hpp"
#include <SDL2/SDL_main.h>
#include "game.hpp"

int main(int argc, char* argv[])
{
	SDLWrapper* wrapper = new SDLWrapper{ "The butler didn't do it", 800, 800, .8f };
	Game game{};

	for (int i = 1; i < argc; i++)
	{
		if (game.debug == false)
		{
			game.debug = std::string(argv[i]) == "debug";
			std::cout << "=========== RUNNING WITH DEBUG ENABLED ===========" << std::endl;
		}
	}

	game.OnStart();
	while (SDLWrapper::Update())
	{
		game.OnUserUpdate(SDLWrapper::deltaTime());
	}

	delete wrapper;

	return 0;
}