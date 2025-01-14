#include "game.hpp"
#include "loader.hpp"

#define YAML_DEF
#include "../libs/yaml.hpp"

void Loader::LoadSuspects()
{
	std::vector<std::string> suspectNames{};
	std::vector<std::vector<std::string>> suspectMotives{};
	std::cout << "Loading suspects..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/suspects.yaml");

	std::string sprName = "";
	int sprWidth = 0, sprHeight = 0, sprCols = 0, sprRows = 0;
	data->killer = 0;
	for (auto it = root.Begin(); it != root.End(); it++)
	{
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
					if (suspectMotives.size() < data->killer + 1) suspectMotives.push_back(std::vector<std::string>{});
					suspectMotives.at(data->killer).push_back((*m).second.As<std::string>());
				}

				data->killer++;
			}
		}
	}

	data->suspectSprite.Load(sprName, sprWidth, sprHeight, sprCols, sprRows);
	std::cout << "Done loading suspects. Creating cards." << std::endl;

	for (int i = 0; i < suspectNames.size(); i++) data->suspects.push_back(Suspect{ suspectNames.at(i), data->suspectSprite, i, suspectMotives.at(i), Game::SUSPECT });
}

void Loader::LoadWeapons()
{
	std::vector<std::string> names{};
	std::cout << "Loading weapons..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/weapons.yaml");

	std::string sprName = "";
	int sprWidth = 0, sprHeight = 0, sprCols = 0, sprRows = 0;
	for (auto it = root.Begin(); it != root.End(); it++)
	{
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

	data->weaponSprite.Load(sprName, sprWidth, sprHeight, sprCols, sprRows);
	std::cout << "Done loading weapons. Creating cards." << std::endl;

	for (int i = 0; i < names.size(); i++) data->weapons.push_back({ names.at(i), data->weaponSprite, i, Game::WEAPON });
}

std::vector<Room> Loader::LoadRooms()
{
	std::vector<Room> data;
	std::vector<std::string> rooms{};
	std::cout << "Loading rooms..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/rooms.yaml");

	for (auto it = root.Begin(); it != root.End(); it++)
	{
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

void Loader::LoadIntroScene()
{
	std::cout << "Loading intro scene..." << std::endl;
	std::string spriteDir = "";
	int width, height, cols = 1, rows = 1;

	YAML::Node root;
	YAML::Parse(root, "config/intro.yaml");

	for (auto it = root.Begin(); it != root.End(); it++)
	{
		if ((*it).first == "responses")
		{
			for (auto m = (*it).second.Begin(); m != (*it).second.End(); m++)
			{
				data->introScene.response.push_back({ (*m).second.As<std::string>(), data->responseSprite, 0, Game::NONE });
			}
		}
		else if ((*it).first == "speakerLine") data->introScene.line = (*it).second.As<std::string>();
		else if ((*it).first == "bgSprite")
		{
			data->introScene.background = (*it).second.As<std::string>();
			SDLWrapper::LoadSprite(data->introScene.background);
		}
		else if ((*it).first == "responseSprite")
		{
			spriteDir = (*it).second["name"].As<std::string>();
			width = (*it).second["width"].As<int>();
			height = (*it).second["height"].As<int>();
			cols = (*it).second["col"].As<int>();
			rows = (*it).second["row"].As<int>();
			SDLWrapper::LoadSprite(data->introScene.background);
		}
	}

	data->responseSprite.Load(spriteDir, width, height, cols, rows);
}

void Loader::LoadScene(std::string sceneName)
{
	std::string dir = "config/" + sceneName + ".yaml";
	std::cout << "Loading scene " << dir << "..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, dir.c_str());
	DynamicScene scene{};

	std::string curResponse;
	std::string secondStep;
	std::vector<std::string> outcomes{};

	if (root["speaker"].Type() != 0) scene.speakerInitState = root["speaker"].As<std::string>();
	else std::cout << "CRITICAL ERROR! Cannot load scene because cannot parse speaker!" << std::endl;

	// Load responses
	auto commit = [&]()
		{
			scene.outcomes.emplace(scene.response.size(), outcomes);
			scene.response.push_back(Card{ curResponse, data->responseSprite, 0, Game::NONE });
			scene.secondStep.push_back(secondStep);
			// std::cout << "Commiting response " << curResponse << " with " << std::to_string(outcomes.size()) << " outcomes" << std::endl;
			outcomes.clear();
			curResponse.clear();
			secondStep.clear();
		};

	if (root["responses"].Type() != 0)
	{
		for (auto it = root["responses"].Begin(); it != root["responses"].End(); it++)
		{
			if (outcomes.size() > 0) commit();
			curResponse = (*it).second["prompt"].As<std::string>();
			if ((*it).second["secondStep"].Type() != 0) secondStep = (*it).second["secondStep"].As<std::string>();
			if ((*it).second["outcomes"].Type() != 0)
			{
				for (auto m = (*it).second["outcomes"].Begin(); m != (*it).second["outcomes"].End(); m++)
				{
					outcomes.push_back((*m).second.As<std::string>());
				}
			}
		}
		commit();
		data->scenes.emplace(sceneName, scene);
	}
	else std::cout << "CRITICAL ERROR! Cannot load scene because cannot parse responses!" << std::endl;
}

bool Loader::LoadPackage(int& s)
{
	if (s == 0)
	{
		SDLWrapper::DrawString("Loading... Creating gamepack");
		data = new GamePack();
	}
	else if (s == 1)
	{
		SDLWrapper::DrawString("Loading... Loading suspects");
		LoadSuspects();
	}
	else if (s == 2)
	{
		SDLWrapper::DrawString("Loading... Loading weapons");
		LoadWeapons();
	}
	else if (s == 3)
	{
		SDLWrapper::DrawString("Loading... Loading rooms");
		data->rooms = LoadRooms();
		LoadIntroScene();
		LoadScene("conversation");
	}
	else if (s == 4)
	{
		SDLWrapper::DrawString("Loading... Loading generics");
		// TODO: Load the player using yaml
		SDLWrapper::LoadSprite("sprites/player.png");

		// TODO: Load sounds
	}
	else if (s == 5)
	{
		SDLWrapper::DrawString("Loading... Randomizing");
		std::cout << "Shuffling..." << std::endl;
		srand(static_cast<unsigned int>(time(NULL)));
		data->killer = rand() % data->suspects.size();
		data->weapon = rand() % data->weapons.size();

		data->suspects.at(data->killer).isKiller = true;

		std::cout << "Picked killer and weapon. Begin game!" << std::endl;
	}
	else if (s == 6)
	{
		SDLWrapper::DrawString("Loading... Applying data!");
		data->mapView = new MapView(data->suspects, data->rooms);
		data->mapView->weapon = data->weapons.at(data->weapon).name;
	}
	else return true;

	SDLWrapper::DrawRect(0, SDLWrapper::getScreenHeight() - 30, SDLWrapper::getScreenWidth() * (static_cast<float>(s) / 6.0f), 30);
	s++;
	return false;
}

Loader::GamePack* Loader::getData() { return data; }