#include "game.hpp"

#define YAML_DEF
#include "../libs/yaml.hpp"

void Clue::LoadSuspects()
{
	std::vector<std::string> suspectNames{};
	std::vector<std::vector<std::string>> suspectMotives{};
	std::cout << "Loading suspects..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/suspects.yaml");

	std::string sprName = "";
	int sprWidth = 0, sprHeight = 0, sprCols = 0, sprRows = 0;
	killer = 0;
	// Iterate second sequence item.
	for (auto it = root.Begin(); it != root.End(); it++)
	{
		// std::cout << (*it).first << ": " << std::to_string((*it).second.As<int>()) << std::endl;
		if ((*it).first == "width") sprWidth = (*it).second.As<int>();
		else if ((*it).first == "height") sprHeight = (*it).second.As<int>();
		else if ((*it).first == "cols") sprCols = (*it).second.As<int>();
		else if ((*it).first == "spriteName") sprName = (*it).second.As<std::string>();
		else if ((*it).first == "rows") sprRows = (*it).second.As<int>();
		else if ((*it).first == "characters")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
			{
				suspectNames.push_back((*o).second["name"].As<std::string>());
				for (auto m = (*o).second["motives"].Begin(); m != (*o).second["motives"].End(); m++)
				{
					if (suspectMotives.size() < killer + 1) suspectMotives.push_back(std::vector<std::string>{});
					suspectMotives.at(killer).push_back((*m).second.As<std::string>());
				}

				killer++;
			}
		}
	}

	suspectSprite.Load(sprName, sprWidth, sprHeight, sprCols, sprRows);
	std::cout << "Done loading suspects. Creating cards." << std::endl;

	for (int i = 0; i < suspectNames.size(); i++) suspects.push_back(Suspect{ suspectNames.at(i), suspectSprite, i, suspectMotives.at(i), Clue::SUSPECT });
}

void Clue::LoadWeapons()
{
	std::vector<std::string> names{};
	std::cout << "Loading weapons..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/weapons.yaml");

	std::string sprName = "";
	int sprWidth = 0, sprHeight = 0, sprCols = 0, sprRows = 0;
	// Iterate second sequence item.
	for (auto it = root.Begin(); it != root.End(); it++)
	{
		// std::cout << (*it).first << ": " << std::to_string((*it).second.As<int>()) << std::endl;
		if ((*it).first == "width") sprWidth = (*it).second.As<int>();
		else if ((*it).first == "height") sprHeight = (*it).second.As<int>();
		else if ((*it).first == "cols") sprCols = (*it).second.As<int>();
		else if ((*it).first == "rows") sprRows = (*it).second.As<int>();
		else if ((*it).first == "spriteName") sprName = (*it).second.As<std::string>();
		else if ((*it).first == "names")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
				names.push_back((*o).second.As<std::string>());
		}
	}

	weaponSprite.Load(sprName, sprWidth, sprHeight, sprCols, sprRows);
	std::cout << "Done loading weapons. Creating cards." << std::endl;

	for (int i = 0; i < names.size(); i++) weapons.push_back({ names.at(i), weaponSprite, i, Clue::WEAPON });
}

std::vector<Room> Clue::LoadRooms()
{
	std::vector<Room> data;
	std::vector<std::string> rooms{};
	std::cout << "Loading rooms..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/rooms.yaml");

	// Iterate second sequence item.
	for (auto it = root.Begin(); it != root.End(); it++)
	{
		// std::cout << (*it).first << ": " << std::to_string((*it).second.As<int>()) << std::endl;
		if ((*it).first == "rooms")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
			{
				std::string sprite = "";
				if ((*o).second["sprite"].Type() != 0)
				{
					sprite = (*o).second["sprite"].As<std::string>();
					SDLWrapper::LoadSprite(sprite);
				}
				if ((*o).second["char"].Type() != 0)
				{
					data.push_back(Room{ .index = (*o).second["char"].As<char>(), .name = (*o).second["name"].As<std::string>(), .sprite = sprite });
				}
				else rooms.push_back((*o).second["name"].As<std::string>()); // Only push room name to list if it wont appear in the game map
			}
		}
	}

	std::cout << "Done loading rooms.." << std::endl;
	return data;
}

void Clue::LoadIntroScene()
{
	std::cout << "Loading intro scene..." << std::endl;
	responseSprite.Load("sprites/response.png", 64, 64, 1, 1);

	YAML::Node root;
	YAML::Parse(root, "config/intro.yaml");

	// Iterate second sequence item.
	for (auto it = root.Begin(); it != root.End(); it++)
	{
		if ((*it).first == "responses")
		{
			for (auto m = (*it).second.Begin(); m != (*it).second.End(); m++)
			{
				introScene.response.push_back({ (*m).second.As<std::string>(), responseSprite, 0, Clue::NONE });
			}
		}
		else if ((*it).first == "speakerLine") introScene.line = (*it).second.As<std::string>();
		else if ((*it).first == "bgSprite")
		{
			introScene.background = (*it).second.As<std::string>();
			SDLWrapper::LoadSprite(introScene.background);
		}
	}
}

void Clue::LoadData(std::vector<Room>& rooms)
{
	LoadSuspects();
	LoadWeapons();
	rooms = LoadRooms();
	LoadIntroScene();

	// TODO: Load the player using yaml
	SDLWrapper::LoadSprite("sprites/player.png");

	// TODO: Load sounds
}