#pragma once
#include "suspect.hpp"
#include "SDLWrapper.hpp"
#include <iostream>
#include <vector>

struct Room
{
	char index;
	std::string name;
	std::string sprite;
	std::vector<std::string> components{};
	std::vector<gobl::vec2i> standOffs{};
};

class MapView
{
	const int MAP_SCALE = 24;
	char playerRoom;
	bool playerKilled = false;
	int interviewing = -1;
	char weaponRoom = '.';
	char murderRoom = '.';
	bool foundMurderRoom = false;

	std::vector<Suspect>& suspects;
	std::vector<char> suspectPos{};
	std::vector<int> roomOccupation{};

	std::vector<Room> rooms{};
	Room* getRoom(const char&);
	int getRoomIndex(const char& c);

	void DrawCharacters(float deltaTime);

public:
	std::string weapon = "";
	std::string GetMurderRoom();

	int Display(float deltaTime);

	void DrawRoom(std::string name);

	MapView(std::vector<Suspect>& suspects, std::vector<Room>& roomNames);
};