#include "mapView.hpp"
#include "SDLWrapper.hpp"

MapView::MapView(std::vector<Suspect>& suspects) : suspects(suspects)
{
	player = 21 + (W * 8);
	playerRoom = static_cast<char>(map[player]);

	do
	{
		weaponRoom = static_cast<char>(map[rand() % (W * H)]);
	} while (weaponRoom == '.');

	do
	{
		murderRoom = static_cast<char>(map[rand() % (W * H)]);
	} while (murderRoom == '.');

	for (int i = 0; i < suspects.size(); i++) suspectPos.push_back(rand() % (W * H));
	foundMurderRoom = false;
}

void MapView::DrawCharacters(float deltaTime)
{
	int peopleInRoom = 0;
	bool killerInRoom = false;
	static float timer = 0.0f;
	const int px = player % W;
	const int py = player / W;

	std::string mouseTip = "";
	for (int i = 0; i < suspects.size(); i++)
	{
		if (static_cast<char>(map[suspectPos[i]]) != playerRoom) continue;

		const int x = suspectPos[i] % W;
		const int y = suspectPos[i] / W;

		suspects[i].DrawMini(gobl::vec2<float>{ (x + 0.5f)* MAP_SCALE, (y + 0.5f)* MAP_SCALE }, MAP_SCALE);

		if (SDLWrapper::getMouse().y > y * MAP_SCALE && SDLWrapper::getMouse().y < (y + 1.5f) * MAP_SCALE && SDLWrapper::getMouse().x > x * MAP_SCALE && SDLWrapper::getMouse().x < (x + 1.5f) * MAP_SCALE)
		{
			mouseTip = suspects[i].name;
			if (SDLWrapper::getMouse().bDown(0)) interviewing = i;
		}

		killerInRoom = suspects[i].isKiller;
		peopleInRoom++;
	}

	if (mouseTip.size() > 0)
	{
		int my = SDLWrapper::getMouse().y - 12;
		SDLWrapper::DrawString(mouseTip, { SDLWrapper::getMouse().x, my }, sdl::WHITE);
	}

	if (peopleInRoom == 1 && killerInRoom)
	{
		const float MAX_TIMER = 10.0f;
		SDLWrapper::DrawString("They're being shifty..." + std::to_string(int(ceil(MAX_TIMER - timer))), { 50, 10 }, sdl::WHITE); // FIXME: Don't tell the player this is the killer
		timer += deltaTime;

		if (timer >= MAX_TIMER) playerKilled = true;
	}
	else timer = 0.0f;

	if (weaponRoom == playerRoom && weaponRoom != murderRoom)
	{
		SDLWrapper::DrawString("This must be the weapon... The " + weapon, { px, py }, sdl::WHITE); // FIXME: Don't tell the player this is the weapon
	}
	else if (murderRoom == playerRoom && weaponRoom != murderRoom)
	{
		SDLWrapper::DrawString("This must be where the victem was killed...", { px, py }, sdl::WHITE);
		foundMurderRoom = true;
	}
	else if (murderRoom == playerRoom && weaponRoom == murderRoom)
	{
		SDLWrapper::DrawString("The victem was kill here with the " + weapon, { px, py }, sdl::WHITE);
		foundMurderRoom = true;
	}

	// Draw player
	SDLWrapper::DrawSprite("sprites/player.png", gobl::vec2<float>{ float(px) * MAP_SCALE, float(py) * MAP_SCALE }, gobl::vec2<int>{MAP_SCALE, MAP_SCALE});
}

void MapView::MoveCharacter(int& character, int x, int y)
{
	if (x < 0 && character % W >(character - 1) % W) character -= 1;
	if (x > 0 && character % W < (character + 1) % W) character += 1;
	if (y < 0 && character > W) character -= W;
	if (y > 0 && character + W < W * H) character += W;
}

int MapView::Display(float deltaTime)
{
	interviewing = -1;
	auto& input = SDLWrapper::getKeyboard();

	if (input.bDown(SDLK_LEFT)) MoveCharacter(player, -1, 0);
	if (input.bDown(SDLK_RIGHT)) MoveCharacter(player, 1, 0);
	if (input.bDown(SDLK_UP)) MoveCharacter(player, 0, -1);
	if (input.bDown(SDLK_DOWN)) MoveCharacter(player, 0, 1);

	playerRoom = static_cast<char>(map[player]);
	for (int x = 0; x < W; x++)
	{
		for (int y = 0; y < H; y++)
		{
			SDL_Color tileCol = sdl::VERY_DARK_MAGENTA;
			const int index = y * W + x;

			switch (map[index])
			{
			case 's': tileCol = sdl::VERY_DARK_GREEN; break;
			case 'h': tileCol = sdl::DARK_YELLOW; break;
			case 'l': tileCol = sdl::DARK_GREY; break;
			case 'b': tileCol = sdl::VERY_DARK_BLUE; break;
			case 'f': tileCol = sdl::VERY_DARK_YELLOW; break;
			case 'p': tileCol = sdl::DARK_RED; break;
			case 'd': tileCol = sdl::VERY_DARK_RED; break;
			case 'k': tileCol = sdl::DARK_GREEN; break;
			case 'c': tileCol = sdl::DARK_BLUE; break;
			}

			if (map[index] != playerRoom && playerRoom != '.')
			{
				tileCol = sdl::VERY_DARK_MAGENTA;
			}
			if (playerRoom == '.' && map[index] != '.') tileCol = sdl::VERY_DARK_GREY;

			SDLWrapper::DrawRect(x * MAP_SCALE, y * MAP_SCALE, MAP_SCALE, MAP_SCALE, tileCol);
		}
	}

	DrawCharacters(deltaTime);
	SDLWrapper::DrawString(roomNames[playerRoom], gobl::vec2<int>{ 10, (int)SDLWrapper::getScreenHeight() - 8 });

	const float MOVE_TIME = 1.0f;
	static float moveTimer = 0;
	moveTimer += deltaTime;
	if (moveTimer >= MOVE_TIME)
	{
		moveTimer = 0.0f;
		for (auto& s : suspectPos)
		{
			MoveCharacter(s, -1 + (rand() % 3), -1 + (rand() % 3));
		}
	}

	if (interviewing > -1) return interviewing + 1;
	if (playerKilled) return -1;

	return 0;
}

std::string MapView::GetMurderRoom()
{
	return foundMurderRoom ? roomNames[murderRoom] : "???";
}