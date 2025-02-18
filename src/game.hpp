#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

#include "card.hpp"
#include "questionObject.hpp"
#include "suspect.hpp"
#include "mapView.hpp"
#include "button.hpp"

#include "loader.hpp"

class Game
{
public:
	enum HoldingType { NONE, SUSPECT, WEAPON };
	bool debug = false;

	std::string title = "A video game";
	unsigned int windowWidth = 800, windowHeight = 800;
	float renderScale = 1.0f;

	Game();
	~Game();

	void OnStart();
	bool OnUserUpdate(float deltaTime);
private:
	std::vector<QuestionObject> questions{
		{.text = "Who?", .pos = {50, 100}},
		{.text = "What?", .pos = {300, 100}},
		{.text = "Where?", .pos = {75, 300}},
		{.text = "Why?", .pos = {325, 300}}
	};

	int interviewing = 0;

	Loader::GamePack* gameData = nullptr;

	enum GameState : uint8_t
	{
		// User stuff
		Introduction,
		Interviewing,
		Accusing,
		Investigating,
		Win,
		Lose,

		// Developer stuff
		RoomEditing,
	};

	GameState state = Introduction;

	Loader loader{};
	bool loaded = false;

	int holdIndex = -1;
	HoldingType holding = NONE;

	void DisplayAccusing();
	void DisplayKiller(bool foundKiller);
	void DisplayInterview(float deltaTime);
	void DisplayIntroduction(float deltaTime);
	void DisplayRoomEditor(float deltaTime);
	std::string editorFileDrop;
	void OnFileDropped(std::string);
};