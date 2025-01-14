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

	std::string mouseTip = "";
	for (int i = 0; i < suspects.size(); i++)
	{
		if (suspectPos[i] != playerRoom) continue;

		// FIXME: We need some positions for suspects to stand
		const int x = 200;
		const int y = 200;

		suspects[i].DrawMini(gobl::vec2f{ static_cast<float>(x * MAP_SCALE), SDLWrapper::getScreenHeight() - 200.0f }, 100);

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
		SDLWrapper::DrawString("This must be the weapon... The " + weapon, { 0, 0 }, sdl::BLACK); // FIXME: Don't tell the player this is the weapon
	}
	else if (murderRoom == playerRoom && weaponRoom != murderRoom)
	{
		SDLWrapper::DrawString("This must be where the victem was killed...", { 0, 0 }, sdl::BLACK);
		foundMurderRoom = true;
	}
	else if (murderRoom == playerRoom && weaponRoom == murderRoom)
	{
		SDLWrapper::DrawString("The victem was kill here with the " + weapon, { 0, 0 }, sdl::BLACK);
		foundMurderRoom = true;
	}
}

bool isNavable(const Room& room)
{
	for (auto& c : room.components) if (c == "navable") return true;
	return false;
}

int MapView::Display(float deltaTime)
{
	static gobl::vec2i lastInput = {};
	interviewing = -1;
	auto& input = SDLWrapper::getKeyboard();

	if (getRoom(playerRoom) != nullptr && getRoom(playerRoom)->sprite.size() > 0) // FIXME: draw all the characters that are in the current room
	{
		if (input.bDown(SDLK_TAB) || input.bDown(SDLK_SPACE))
		{
			playerRoom = '.';
		}
		DrawRoom(playerRoom);
		DrawCharacters(deltaTime);
	}
	else
	{
		SDLWrapper::getMouse().visible = false;
		SDLWrapper::DrawSprite("sprites/mapScreen.png");

		int y = 200;
		for (auto& r : rooms)
		{
			if (isNavable(r) == false) continue;
			SDL_Color col = sdl::BLACK;

			if (SDLWrapper::getMouse().x > 200 && SDLWrapper::getMouse().x < 600 && SDLWrapper::getMouse().y >= y && SDLWrapper::getMouse().y <= y + 50)
			{
				col = sdl::DARK_GREY;

				if (SDLWrapper::getMouse().bRelease(0))
				{
					playerRoom = r.index; // Transport the player to that room
				}
			}
			SDLWrapper::DrawString(r.name, { 200, y }, col, 32);
			y += 50;
		}

		SDLWrapper::DrawSprite("sprites/mapScreenPen.png", gobl::vec2f{ SDLWrapper::getMouse().x - 20.0f,  SDLWrapper::getMouse().y - 40.0f });
	}

	const float MOVE_TIME = 30.0f; // Move every x seconds
	static float moveTimer = 0;
	moveTimer += deltaTime;
	if (moveTimer >= MOVE_TIME)
	{
		moveTimer = 0.0f;
		for (auto& s : suspectPos)
		{
			s = rooms.at((rand() % rooms.size())).index;
		}
	}

	if (interviewing > -1) return interviewing + 1;
	if (playerKilled) return -1;

	return 0;
}

void MapView::DrawRoom(char index)
{
	SDLWrapper::DrawSprite(getRoom(index)->sprite, {});
}

void MapView::DrawRoom(std::string name)
{
	for (auto& r : rooms) // FIXME: Make this faster by just storing the index and name
	{
		if (r.name == name)
		{
			SDLWrapper::DrawSprite(r.sprite, {});
			return;
		}
	}

	std::cout << "Unable to draw room (" << name << ") could not find it." << std::endl;
}

std::string MapView::GetMurderRoom()
{
	return foundMurderRoom ? getRoom(murderRoom)->name : "???";
}

MapView::MapView(std::vector<Suspect>& suspects, std::vector<Room>& rooms) : suspects(suspects), rooms(rooms)
{
	SDLWrapper::LoadSprite("sprites/mapScreen.png");
	SDLWrapper::LoadSprite("sprites/mapScreenPen.png");

	playerRoom = rooms.at(0).index;

	do
	{
		weaponRoom = rooms.at((rand() % rooms.size())).index;
	} while (weaponRoom == '.');

	do
	{
		murderRoom = rooms.at((rand() % rooms.size())).index;
	} while (murderRoom == '.');

	for (int i = 0; i < suspects.size(); i++) suspectPos.push_back(rooms.at((rand() % rooms.size())).index);
	foundMurderRoom = false;
}