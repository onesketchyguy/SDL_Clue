#pragma once
#include "suspect.hpp"
#include <iostream>

struct Room
{
	char index;
	std::string name;
	std::string sprite;
};

class MapView
{
	const int MAP_SCALE = 24;
	char playerRoom;
	int player = 0;
	bool playerKilled = false;
	int interviewing = -1;
	char weaponRoom = '.';
	char murderRoom = '.';
	bool foundMurderRoom = false;

	std::vector<Suspect>& suspects;
	std::vector<int> suspectPos{};

	const int W = 22, H = 24;
	const wchar_t* map =
		L"ssssss..hhhhhh...lllll"
		"ssssss..hhhhhh...lllll"
		"ssssss..hhhhhh...lllll"
		"ssssss..hhhhhh...lllll"
		"........hhhhhh...lllll"
		"........hhhhhh...lllll"
		"bbbbb................."
		"bbbbbb................"
		"bbbbbb................"
		"bbbbbb................"
		"bbbbb..........fffffff"
		"...............fffffff"
		"ppppp..........fffffff"
		"ppppp..........fffffff"
		"ppppp..........fffffff"
		"ppppp..........fffffff"
		"ppppp.............ffff"
		".......dddddddd......."
		".......dddddddd..kkkkk"
		"cccc...dddddddd..kkkkk"
		"ccccc..dddddddd..kkkkk"
		"ccccc..dddddddd..kkkkk"
		"ccccc..dddddddd..kkkkk"
		"ccccc....dddd....kkkkk";

	std::vector<Room> rooms{};
	Room* getRoom(const char&);

	void DrawCharacters(float deltaTime);
	void MoveCharacter(int& character, int x, int y);

public:
	std::string weapon = "";
	std::string GetMurderRoom();

	int Display(float deltaTime);

	MapView(std::vector<Suspect>& suspects, std::vector<Room>& roomNames);
};