#pragma once
#include "portal.h"
#include "suspect.hpp"

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

	std::map<char, std::string> roomNames{
		{'s', "Study"},
		{'h', "Hall"},
		{'l', "Lounge"},
		{'b', "Library"},
		{'f', "Dining room"},
		{'p', "Billiard Room"},
		{'d', "Ball room"},
		{'k', "Kitchen"},
		{'c', "Conservatory"},
		{'.', "Hallway" }
	};

	void DrawCharacters(float deltaTime);
	void MoveCharacter(int& character, int x, int y);

public:
	std::string weapon = "";
	std::string GetMurderRoom();

	MapView(std::vector<Suspect>& suspects);

	int Display(float deltaTime);
};