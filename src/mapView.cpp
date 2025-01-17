#include "mapView.hpp"
#include "SDLWrapper.hpp"

Room* MapView::getRoom(const char& n)
{
	for (auto& r : rooms) if (r.index == n) return &r;
	return nullptr;
}

int MapView::getRoomIndex(const char& c)
{
	for (int i = 0; i < rooms.size(); i++) if (rooms.at(i).index == c) return i;
	return -1;
}

void MapView::DrawCharacters(float deltaTime)
{
	int peopleInRoom = 0;
	bool killerInRoom = false;
	static float timer = 0.0f;
	int inRoom = 0;

	std::string cursorTip = "";
	for (int i = 0; i < suspects.size(); i++)
	{
		if (suspectPos[i] != playerRoom) continue;

		gobl::vec2i miniPos{};
		int scale = 200;
		int roomIndex = getRoomIndex(suspectPos[i]);
		if (roomIndex != -1)
		{
			if (inRoom >= rooms.at(roomIndex).standOffs.size())
			{
				throw std::exception("Too many in room and not enough standoffs!");
			}
			miniPos = rooms.at(roomIndex).standOffs.at(inRoom);
			scale = rooms.at(roomIndex).standScales.at(inRoom);
			inRoom++;
		}

		suspects[i].DrawMini(gobl::vec2f{ static_cast<float>(miniPos.x), static_cast<float>(miniPos.y) }, scale);

		if (SDLWrapper::getMouse().y > miniPos.y && SDLWrapper::getMouse().y < miniPos.y + scale &&
			SDLWrapper::getMouse().x > miniPos.x && SDLWrapper::getMouse().x < miniPos.x + scale)
		{
			cursorTip = suspects[i].name;
			if (SDLWrapper::getMouse().bDown(0)) interviewing = i;
		}

		killerInRoom = suspects[i].isKiller;
		peopleInRoom++;
	}

	for (int i = 0; i < rooms.at(getRoomIndex(playerRoom)).props.size(); i++)
	{
		auto& prop = rooms.at(getRoomIndex(playerRoom)).props.at(i);
		SDLWrapper::DrawSprite(prop.sprite.name, prop.pos, gobl::vec2i{ prop.scale, prop.scale }); // TODO: Allow the designer to set the order of props/characters
	}

	if (cursorTip.size() > 0)
	{
		int my = SDLWrapper::getMouse().y - 12;
		SDLWrapper::DrawString(cursorTip, { SDLWrapper::getMouse().x, my }, sdl::BLACK);
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
	dt = deltaTime;
	static gobl::vec2i lastInput = {};
	interviewing = -1;
	auto& input = SDLWrapper::getKeyboard();

	if (getRoom(playerRoom) != nullptr && getRoom(playerRoom)->sprite.size() > 2) // FIXME: draw all the characters that are in the current room
	{
		DrawRoom(getRoom(playerRoom)->name, true);

		// Leave the room
		if (input.bDown(SDLK_TAB) || input.bDown(SDLK_SPACE)) playerRoom = '.';
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
		roomOccupation.clear();
		roomOccupation.resize(rooms.size());
		moveTimer = 0.0f;
		for (auto& s : suspectPos)
		{
			int i;
			do
			{
				i = (rand() % rooms.size());
			} while (isNavable(rooms.at(i)) == false);

			s = rooms.at(i).index;
			roomOccupation.at(i)++;
		}
	}

	if (interviewing > -1) return interviewing + 1;
	if (playerKilled) return -1;

	return 0;
}

void MapView::DrawRoom(std::string name, bool populate)
{
	if (name.size() < 2)
	{
		SDLWrapper::DrawSprite(rooms.at(getRoomIndex(playerRoom)).sprite);
	}
	else
	{
		bool found = false;
		for (auto& r : rooms) // FIXME: Make this faster by just storing the index and name
		{
			if (r.name == name)
			{
				if (r.sprite.size() < 2) throw std::exception("Tried to draw a room without a sprite!");
				SDLWrapper::DrawSprite(r.sprite);
				found = true;
				break;
			}
		}
		if (!found) std::cout << "Unable to draw room (" << name << ") could not find it." << std::endl;
	}

	if (populate) DrawCharacters(dt);
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