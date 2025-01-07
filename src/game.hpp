#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

#include "card.hpp"
#include "questionObject.hpp"
#include "suspect.hpp"
#include "mapView.hpp"
#include "button.hpp"

// Override base class with your custom functionality
class Clue
{
	std::vector<QuestionObject> questions{
		{.text = "Who?", .pos = {100, 100}},
		{.text = "What?", .pos = {100, 120}},
		{.text = "Where?", .pos = {100, 140}},
		{.text = "Why?", .pos = {100, 160}}
	};

	SpriteData suspectSprite;
	std::vector<Suspect> suspects{};

	SpriteData weaponSprite;
	std::vector<Card> weapons{};

	int killer = 0, weapon = 0, interviewing = 0;

	void LoadSuspects();
	void LoadWeapons();

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
	enum HoldingType { NONE, SUSPECT, WEAPON };
	HoldingType holding = NONE;

	bool DisplaySuspectCards();
	bool DisplayWeaponCards();
	void DisplayAccusing();
	void DisplayKiller(bool foundKiller);
	void DisplayInterview(float deltaTime);
	void DisplayIntroduction(float deltaTime);

public:
	Clue();
public:
	bool OnUserUpdate(float deltaTime);
};
