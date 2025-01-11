#include "mapView.hpp"
#include "SDLWrapper.hpp"

Room* MapView::getRoom(const char& n)
{
	for (auto& r : rooms) if (r.index == n) return &r;
	return nullptr;
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

		//suspects[i].DrawMini(gobl::vec2<float>{ (x + 0.5f)* MAP_SCALE, (y + 0.5f)* MAP_SCALE }, MAP_SCALE);

		suspects[i].DrawMini(gobl::vec2f{x * MAP_SCALE, SDLWrapper::getScreenHeight() - 200}, 100);

		if (SDLWrapper::getMouse().y > SDLWrapper::getScreenHeight() - 200 && SDLWrapper::getMouse().y < SDLWrapper::getScreenHeight() - 150 && 
			SDLWrapper::getMouse().x > x && SDLWrapper::getMouse().x < (x + 1.0f) * 100)
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
		SDLWrapper::DrawString(mouseTip, { SDLWrapper::getMouse().x, my }, sdl::BLACK);
	}

	if (peopleInRoom == 1 && killerInRoom)
	{
		const float MAX_TIMER = 10.0f;
		SDLWrapper::DrawString("They're being shifty..." + std::to_string(int(ceil(MAX_TIMER - timer))), { 50, 10 }, sdl::BLACK); // FIXME: Don't tell the player this is the killer
		timer += deltaTime;

		if (timer >= MAX_TIMER) playerKilled = true;
	}
	else timer = 0.0f;

	if (weaponRoom == playerRoom && weaponRoom != murderRoom)
	{
		SDLWrapper::DrawString("This must be the weapon... The " + weapon, { px, py }, sdl::BLACK); // FIXME: Don't tell the player this is the weapon
	}
	else if (murderRoom == playerRoom && weaponRoom != murderRoom)
	{
		SDLWrapper::DrawString("This must be where the victem was killed...", { px, py }, sdl::BLACK);
		foundMurderRoom = true;
	}
	else if (murderRoom == playerRoom && weaponRoom == murderRoom)
	{
		SDLWrapper::DrawString("The victem was kill here with the " + weapon, { px, py }, sdl::BLACK);
		foundMurderRoom = true;
	}

	// Draw player
	// SDLWrapper::DrawSprite("sprites/player.png", gobl::vec2<float>{ float(px)* MAP_SCALE, float(py)* MAP_SCALE }, gobl::vec2<int>{MAP_SCALE, MAP_SCALE});
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
	static gobl::vec2i lastInput = {};
	interviewing = -1;
	auto& input = SDLWrapper::getKeyboard();

	if (getRoom(playerRoom) != nullptr && getRoom(playerRoom)->sprite.size() > 0) // FIXME: draw all the characters that are in the current room
	{
		SDLWrapper::DrawSprite(getRoom(playerRoom)->sprite, {});
		if (input.bDown(SDLK_TAB) || input.bDown(SDLK_SPACE))
		{
			playerRoom = '.';
		}

		DrawCharacters(deltaTime);
	}
	else
	{
		SDL_ShowCursor(SDL_DISABLE);
		SDLWrapper::DrawSprite("sprites/mapScreen.png");

		int y = 200;
		for (auto& r : rooms)
		{
			if (r.name == "Hallway") continue;
			SDL_Color col = sdl::BLACK;

			if (SDLWrapper::getMouse().x > 200 && SDLWrapper::getMouse().x < 600 && SDLWrapper::getMouse().y >= y && SDLWrapper::getMouse().y <= y + 50)
			{
				col = sdl::DARK_GREY;

				if (SDLWrapper::getMouse().bRelease(0))
				{
					playerRoom = r.index; // Transport the player to that room
				}
			}
			SDLWrapper::DrawString(r.name, {200, y}, col, 32);
			y += 50;
		}

		SDLWrapper::DrawSprite("sprites/mapScreenPen.png", gobl::vec2f{ SDLWrapper::getMouse().x-20,  SDLWrapper::getMouse().y-40 });
	}

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
	return foundMurderRoom ? rooms[murderRoom].name : "???";
}

MapView::MapView(std::vector<Suspect>& suspects, std::vector<Room>& rooms) : suspects(suspects), rooms(rooms)
{
	SDLWrapper::LoadSprite("sprites/mapScreen.png");
	SDLWrapper::LoadSprite("sprites/mapScreenPen.png");

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