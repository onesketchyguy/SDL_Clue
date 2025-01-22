#include "game.hpp"
#include "loader.hpp"

#define YAML_DEF
#include "../libs/yaml.hpp"

void ParseSprite(YAML::Node& node, SpriteData& data)
{
	data.Load(node["name"].As<std::string>(), node["width"].As<int>(), node["height"].As<int>(), node["cols"].As<int>(), node["rows"].As<int>());
}

void SerializeSprite(YAML::Node& root, SpriteData& data)
{
	root["sprite"]["name"] = data.name;
	root["sprite"]["cols"] = data.cols;
	root["sprite"]["rows"] = data.rows;
	root["sprite"]["width"] = data.width;
	root["sprite"]["height"] = data.height;
}

void Loader::SaveSuspects()
{
	YAML::Node root;
	SerializeSprite(root, data->suspectSprite);

	int c = 0;
	for (auto& s : data->suspects)
	{
		root["characters"].PushBack();
		root["characters"][c]["name"] = s.name;
		root["characters"][c]["sprIndex"] = s.sprIndex;
		root["characters"][c]["motives"] = s.getMotives();
		c++;
	}

	YAML::Serialize(root, "config/suspects.yaml");
}

void Loader::LoadSuspects()
{
	if (debug) std::cout << "Loading suspects..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/suspects.yaml");

	for (auto it = root.Begin(); it != root.End(); it++)
	{
		if ((*it).first == "sprite") ParseSprite((*it).second, data->suspectSprite);
		else if ((*it).first == "characters")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
			{
				std::string name = (*o).second["name"].As<std::string>();
				int sprIndex = (*o).second["sprIndex"].As<int>();
				std::vector<std::string> suspectMotives{};
				for (auto m = (*o).second["motives"].Begin(); m != (*o).second["motives"].End(); m++)
				{
					suspectMotives.push_back((*m).second.As<std::string>());
				}

				// Make and push back the card in one go
				data->suspects.push_back(Suspect{ name, data->suspectSprite, sprIndex, suspectMotives, Game::SUSPECT });
			}
		}
	}

	if (debug) std::cout << "Done loading suspects." << std::endl;
}

void Loader::SaveWeapons()
{
	if (debug) std::cout << "Saving weapons..." << std::endl;

	YAML::Node root;

	SerializeSprite(root, data->weaponSprite);

	std::vector<std::string> names{};
	for (auto& w : data->weapons) names.push_back(w.name);

	root["names"] = names;

	YAML::Serialize(root, "config/weapons.yaml");
}

void Loader::LoadWeapons()
{
	std::vector<std::string> names{};
	if (debug) std::cout << "Loading weapons..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/weapons.yaml");

	for (auto it = root.Begin(); it != root.End(); it++)
	{
		if ((*it).first == "sprite")
		{
			ParseSprite((*it).second, data->weaponSprite);
		}
		else if ((*it).first == "names")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
				names.push_back((*o).second.As<std::string>());
		}
	}

	if (debug) std::cout << "Done loading weapons. Creating cards." << std::endl;

	for (int i = 0; i < names.size(); i++) data->weapons.push_back({ names.at(i), data->weaponSprite, i, Game::WEAPON });
}

void Loader::SaveRooms()
{
	if (debug) std::cout << "Saving rooms..." << std::endl;
	YAML::Node root;

	int s = 0;
	for (auto& scene : data->scenes)
	{
		root["scenes"].PushBack();
		root["scenes"][s]["name"] = scene.first;
		s++;
	}

	root["scenes"].PushBack();
	root["scenes"][s]["name"] = "intro"; // Always push the intro into the list

	int curRoom = 0;
	for (auto& roomData : data->rooms)
	{
		root["rooms"].PushBack();
		auto& room = root["rooms"][curRoom];
		room["name"] = roomData.name;
		if (roomData.sprite.size() > 2) room["sprite"] = roomData.sprite;
		room["char"] = std::string("'") + roomData.index + std::string("'");

		if (roomData.components.size() > 0)
		{
			int i;
			for (i = 0; i < roomData.components.size(); i++)
			{
				room["components"].PushBack();
				room["components"][i]["type"] = roomData.components.at(i);
			}

			int off = i;
			for (i = 0; i < roomData.standOffs.size(); i++)
			{
				room["components"].PushBack();
				room["components"][off + i]["standoff"]["x"] = roomData.standOffs.at(i).pos.x;
				room["components"][off + i]["standoff"]["y"] = roomData.standOffs.at(i).pos.y;
				room["components"][off + i]["standoff"]["scale"] = roomData.standOffs.at(i).scale;
				room["components"][off + i]["standoff"]["order"] = roomData.standOffs.at(i).order;
			}

			off = i;
			for (i = 0; i < roomData.props.size(); i++)
			{
				room["components"].PushBack();
				room["components"][off + i]["prop"]["scale"] = roomData.props.at(i).scale;
				room["components"][off + i]["prop"]["x"] = roomData.props.at(i).pos.x;
				room["components"][off + i]["prop"]["y"] = roomData.props.at(i).pos.y;
				room["components"][off + i]["prop"]["name"] = roomData.props.at(i).name;
				room["components"][off + i]["prop"]["order"] = roomData.props.at(i).order;
				SerializeSprite(room["components"][off + i]["prop"], roomData.props.at(i).sprite);
			}
		}

		curRoom++;
	}

	YAML::Serialize(root, "config/rooms_test.yaml");
}

std::vector<Room> Loader::LoadRooms()
{
	std::vector<Room> data;
	std::vector<std::string> rooms{};
	if (debug) std::cout << "Loading rooms..." << std::endl;

	YAML::Node root;
	YAML::Parse(root, "config/rooms.yaml");

	auto iterateComponents = [&](YAML::Node& node, std::vector<std::string>& components, std::vector<Standoff>& standOffs, std::vector<Prop>& props)
		{
			int zeros = 0;
			for (auto o = node.Begin(); o != node.End(); o++)
			{
				if ((*o).second.Type() == 0)
				{
					if (zeros >= 100) throw std::exception("YAML Encountered excessive null types!");
					zeros++;
					continue;
				}

				if ((*o).second["standoff"].Type() != 0)
				{
					int y = (*o).second["standoff"]["y"].As<int>(), x = (*o).second["standoff"]["x"].As<int>(), scale = (*o).second["standoff"]["scale"].As<int>();
					standOffs.push_back(Standoff{
						.pos = { x, y },
						.scale = scale,
						// .order = (*o).second["standoff"]["order"].As<short>()
						});

					if (debug) std::cout << "Adding standoff: " << std::to_string(x) << ", " << std::to_string(y) << std::endl;
				}

				if ((*o).second["prop"].Type() != 0)
				{
					props.push_back(Prop{
							.name = (*o).second["prop"]["name"].As<std::string>(),
							.sprite = {},
							.pos = { (*o).second["prop"]["x"].As<int>(), (*o).second["prop"]["y"].As<int>() },
							.scale = (*o).second["prop"]["scale"].As<int>(),
							// .order = (*o).second["prop"]["order"].As<short>()
						});

					ParseSprite((*o).second["prop"]["sprite"], props.back().sprite);

					if (debug) std::cout << "Adding prop: " << props.back().name << std::endl;
					continue;
				}

				if ((*o).second["type"].Type() != 0)
				{
					if (debug) std::cout << "Adding type component: " << (*o).second["type"].As<std::string>() << std::endl;
					components.push_back((*o).second["type"].As<std::string>());
				}
			}
		};

	for (auto it = root.Begin(); it != root.End(); it++)
	{
		if ((*it).first == "scenes")
		{
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
			{
				if ((*o).second["name"].Type() == 0) continue;
				std::string sceneName = (*o).second["name"].As<std::string>();
				if (debug) std::cout << "Scene detected: " << sceneName << std::endl;
				if (sceneName == "intro") // FIXME: Be more dynamic, don't do this
				{
					LoadIntroScene();
				}
				else
				{
					LoadScene(sceneName);
				}
			}
		}

		if ((*it).first == "rooms")
		{
			std::vector<Standoff> standOffs{};
			std::vector<std::string> components{};
			std::vector<Prop> props{};
			for (auto o = (*it).second.Begin(); o != (*it).second.End(); o++)
			{
				std::string sprite = "";
				if ((*o).second["sprite"].Type() != 0)
				{
					sprite = (*o).second["sprite"].As<std::string>();
					SDLWrapper::LoadSprite(sprite);
				}
				if ((*o).second["components"].Type() != 0)
				{
					iterateComponents((*o).second["components"], components, standOffs, props);
				}

				data.push_back(Room{
					.index = (*o).second["char"].As<char>(),
					.name = (*o).second["name"].As<std::string>(),
					.sprite = sprite,
					.components = components,
					.standOffs = standOffs,
					.props = props
					});
				components.clear();
				standOffs.clear();
				standOffs.clear();
				props.clear();
			}
		}
	}

	if (debug) std::cout << "Done loading rooms.." << std::endl;
	return data;
}

void Loader::LoadIntroScene()
{
	if (debug) std::cout << "Loading intro scene..." << std::endl;
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
		else if ((*it).first == "room")
		{
			data->introScene.room = (*it).second.As<std::string>();
		}
		else if ((*it).first == "responseSprite")
		{
			spriteDir = (*it).second["name"].As<std::string>();
			width = (*it).second["width"].As<int>();
			height = (*it).second["height"].As<int>();
			cols = (*it).second["col"].As<int>();
			rows = (*it).second["row"].As<int>();
		}
	}

	data->responseSprite.Load(spriteDir, width, height, cols, rows);
}

void Loader::LoadScene(std::string sceneName)
{
	std::string dir = "config/" + sceneName + ".yaml";
	if (debug) std::cout << "Loading scene " << dir << "..." << std::endl;

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
			if (debug) std::cout << "Commiting response " << curResponse << " with " << std::to_string(outcomes.size()) << " outcomes" << std::endl;
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
	std::string loading = "Loading...";

	if (s == 0)
	{
		data = new GamePack();
		loading = "Loading... Loading suspects";
	}
	else if (s == 1)
	{
		LoadSuspects();
		loading = "Loading... Loading weapons";
	}
	else if (s == 2)
	{
		LoadWeapons();
		loading = "Loading... Loading rooms";
	}
	else if (s == 3)
	{
		data->rooms = LoadRooms();
		// LoadScene("conversation");
		loading = "Loading... Loading generics";
	}
	else if (s == 4)
	{
		// TODO: Load the player using yaml
		SDLWrapper::LoadSprite("sprites/player.png");

		// TODO: Load sounds

		loading = "Loading... Randomizing";
	}
	else if (s == 5)
	{
		// std::cout << "Shuffling..." << std::endl;
		srand(static_cast<unsigned int>(time(NULL)));
		data->killer = rand() % data->suspects.size();
		data->weapon = rand() % data->weapons.size();
		data->suspects.at(data->killer).isKiller = true;

		//std::cout << "Picked killer and weapon. Begin game!" << std::endl;
		loading = "Loading... Applying data!";
	}
	else if (s == 6)
	{
		data->mapView = new MapView(data->suspects, data->rooms);
		data->mapView->weapon = data->weapons.at(data->weapon).name;
	}
	else return true;

	SDLWrapper::DrawString(loading, { 0, SDLWrapper::getScreenHeight() - 50 });
	SDLWrapper::DrawRect(0, SDLWrapper::getScreenHeight() - 30, static_cast<int>(SDLWrapper::getScreenWidth() * (static_cast<float>(s) / 6.0f)), 30);
	s++;

	return false;
}

void Loader::SaveData()
{
	SaveSuspects();
	SaveWeapons();
	SaveRooms();

	// TODO: Save scenes too
}

Loader::GamePack* Loader::getData() { return data; }