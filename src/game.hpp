#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

#include "card.hpp"
#include "questionObject.hpp"
#include "suspect.hpp"
#include "mapView.hpp"
#include "button.hpp"

struct StaticScene
{
	std::string background;
	std::string line;
	std::vector<Card> response;
};

// Override base class with your custom functionality
class Clue
{
public:
	enum HoldingType { NONE, SUSPECT, WEAPON };
	Clue();

	bool OnUserUpdate(float deltaTime);
private:
	std::vector<QuestionObject> questions{
		{.text = "Who?", .pos = {150, 100}},
		{.text = "What?", .pos = {150, 140}},
		{.text = "Where?", .pos = {150, 160}},
		{.text = "Why?", .pos = {150, 180}}
	};

	SpriteData responseSprite;
	SpriteData suspectSprite;
	SpriteData weaponSprite;

	std::vector<Suspect> suspects{};
	std::vector<Card> weapons{};

	int killer = 0, weapon = 0, interviewing = 0;

	StaticScene introScene{};

	void LoadSuspects();
	void LoadWeapons();
	std::vector<Room> LoadRooms();
	void LoadIntroScene();
	void LoadData(std::vector<Room>& rooms);

	MapView* mapView = nullptr;

	enum GameState : uint8_t
	{
		Introduction,
		Interviewing,
		Accusing,
		Investigating,
		Win,
		Lose
	};

	GameState state = Introduction;

	int holdIndex = -1;
	HoldingType holding = NONE;

	void DisplayAccusing();
	void DisplayKiller(bool foundKiller);
	void DisplayInterview(float deltaTime);
	void DisplayIntroduction(float deltaTime);
};