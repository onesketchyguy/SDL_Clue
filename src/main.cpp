#include "SDLWrapper.hpp"
#include <SDL2/SDL_main.h>
#include "game.hpp"

#undef main // FIXME: Don't do this

int main(int argc, char* argv[])
{
	SDLWrapper* wrapper = new SDLWrapper{ "The butler didn't do it", 800, 800 };

	Loader loader{};
	Game game{};
	int currentStep = 0;
	bool loaded = false;
	while (SDLWrapper::Update())
	{
		if (!loaded) // Wait until loading is completed
		{
			loaded = loader.LoadPackage(currentStep);
			if (loaded == true) game.ApplyData(loader.getData());
		}
		else game.OnUserUpdate(SDLWrapper::deltaTime());
	}

	delete wrapper;

	return 0;
}