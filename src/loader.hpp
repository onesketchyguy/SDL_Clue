#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

#include "card.hpp"
#include "questionObject.hpp"
#include "suspect.hpp"
#include "mapView.hpp"

struct DynamicScene
{
	std::string room;
	std::string line;
	std::vector<Card> response;
	std::map<int, std::vector<std::string>> outcomes;
	std::vector<std::string> secondStep;
	std::string speakerInitState = "";
	std::string speakerState;
	int outcomeState;
	int finalState = -1;
};

class Loader
{
public:
	struct GamePack
	{
		std::vector<Room> rooms;
		MapView* mapView = nullptr;

		int killer = 0, weapon = 0;

		std::vector<Suspect> suspects{};
		std::vector<Card> weapons{};

		SpriteData responseSprite;
		SpriteData suspectSprite;
		SpriteData weaponSprite;

		std::map<std::string, DynamicScene> scenes{};
	};

	bool debug = false;

	bool LoadPackage(int& s);
	GamePack* getData();
	void SaveData();
private:

	void SaveSuspects();
	void LoadSuspects();
	void SaveWeapons();
	void LoadWeapons();
	void SaveRooms();
	std::vector<Room> LoadRooms();

	GamePack* data = nullptr;
};