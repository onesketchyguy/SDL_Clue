#include "game.hpp"

template <typename T>
void HandleProps(std::vector<T>& props, SpriteData* suspectSprite, Room& room, int& _holding, gobl::vec2i& offsetClick, int& typeHolding, const int THIS_TYPE)
{
	static_assert(std::is_base_of<Standoff, T>::value, "T must derive from Standoff");

	for (int i = 0; i < props.size(); i++) // TODO: Allow the designer to set the order of props/characters
	{
		auto& prop = props.at(i);
		SDL_Color tint = sdl::WHITE;

		if (typeHolding == -1 || typeHolding == THIS_TYPE)
		{
			if (SDLWrapper::getMouse().x >= prop.pos.x && SDLWrapper::getMouse().x <= prop.pos.x + prop.scale &&
				SDLWrapper::getMouse().y >= prop.pos.y && SDLWrapper::getMouse().y <= prop.pos.y + prop.scale)
			{
				if (SDLWrapper::getMouse().bHeld(0) && _holding == -1)
				{
					offsetClick.x = prop.pos.x - SDLWrapper::getMouse().x;
					offsetClick.y = prop.pos.y - SDLWrapper::getMouse().y;
					_holding = i;
					typeHolding = THIS_TYPE;
				}

				tint = sdl::GREY;

				if (SDLWrapper::getMouse().wheel != 0.0f)
				{
					if (SDLWrapper::getKeyboard().bHeld(SDLK_LSHIFT))
					{
						if (SDLWrapper::getMouse().wheel < 0)
						{
							int order = prop.order;
							if (prop.order + static_cast<int>(SDLWrapper::getMouse().wheel * 10.0f) > order) prop.order = 0;
						}
						else prop.order += static_cast<int>(SDLWrapper::getMouse().wheel * 10.0f);
					}
					else
					{
						int v = static_cast<int>(SDLWrapper::getMouse().wheel * 10.0f);
						prop.scale += v;

						float centerX = prop.scale / (prop.pos.x - SDLWrapper::getMouse().x),
						centerY = prop.scale / (prop.pos.y - SDLWrapper::getMouse().y);

						prop.pos.x += v / centerX;
						prop.pos.y += v / centerY;
					}
				}

				if (SDLWrapper::getMouse().bDown(2) && SDLWrapper::getKeyboard().bHeld(SDLK_LSHIFT))
				{
					// Delete this prop
					if (props.size() == 1)
					{
						props.clear();
						break;
					}

					std::vector<T> newProps{};
					for (int r = 0; r < props.size(); r++)
						if (r != i) newProps.push_back(props.at(r));

					props = newProps;
					break;
				}

				SDLWrapper::DrawRect(prop.pos.x, prop.pos.y, prop.scale, prop.scale, SDL_Color(255, 0, 0, 50));
			}
			else SDLWrapper::OutlineRect(prop.pos.x, prop.pos.y, prop.scale, prop.scale, sdl::RED);
		}

		if (THIS_TYPE == 0) // Draw prop type
		{
			SDLWrapper::DrawSprite(room.props.at(i).sprite.name, prop.pos, gobl::vec2i{prop.scale, prop.scale}, tint);

			SDLWrapper::DrawString("prop: " + room.props.at(i).name, prop.pos, sdl::BLACK);
			SDLWrapper::DrawString(std::to_string(prop.pos.x) + ", " + std::to_string(prop.pos.y), prop.pos + gobl::vec2i{0, 12}, sdl::BLACK);
		}
		else if (THIS_TYPE == 2) // Draw standoff type
		{
			int susX = i % suspectSprite->cols;
			int susY = i / suspectSprite->cols;
			auto& s = room.standOffs.at(i);

			SDLWrapper::DrawSprite(suspectSprite->name, s.pos, gobl::vec2i{ s.scale, s.scale },
				gobl::vec2<int>{ susX* suspectSprite->width, susY* suspectSprite->height },
				gobl::vec2<int>{ suspectSprite->width, suspectSprite->height },
				tint, s.order);

			SDLWrapper::DrawString("standOff: " + std::to_string(prop.pos.x) + ", " + std::to_string(prop.pos.y), prop.pos, sdl::BLACK);
		}
		else throw std::runtime_error("Invalid type for HandleProps");
	}

	if (_holding != -1 && typeHolding == THIS_TYPE)
	{
		auto& s = props.at(_holding);
		s.pos.x = SDLWrapper::getMouse().x + offsetClick.x;
		s.pos.y = SDLWrapper::getMouse().y + offsetClick.y;
	}
	else
	{
		if (THIS_TYPE == 2 && SDLWrapper::getMouse().bRelease(0) && SDLWrapper::getKeyboard().bHeld(SDLK_LSHIFT)) // Add a standoff
		{
			room.standOffs.push_back(Standoff{ .pos = SDLWrapper::getMousePos() });
		}
	}
}

void Game::DisplayRoomEditor(float deltaTime)
{
	static int curEdit = -1;
	static bool deleteRoom = false;
	SDLWrapper::SetClear(SDL_Color(0, 0, 0, 255));

	auto DrawPanel = [&]()
		{
			SDLWrapper::DrawString("Select room:");

			int y = 16, x = 50;
			for (int i = 0; i < gameData->rooms.size(); i++)
			{
				Button btn{ .onClick = [&] {
						curEdit = i;
					}, .text = gameData->rooms.at(i).name, .pos = {x, y} };

				btn.Draw();
				y += 20;
			}

			Button saveBtn{ .onClick = [&] {
				loader.SaveData();
			}, .text = "[save changes]", .pos = { 200, 0} };
			saveBtn.Draw();

			Button exitBtn{ .onClick = [&] {
				state = GameState::Investigating;
			}, .text = "[exit editor]", .pos = { saveBtn.getEnd().x, 0} };
			exitBtn.Draw();
		};

	if (deleteRoom)
	{
		Button confirmBtn{ .onClick = [&] {
			 std::vector<Room> newRooms{};

			 for (int i = 0; i < gameData->rooms.size(); i++)
			 {
				 if (i == curEdit) continue;
				 newRooms.push_back(gameData->rooms.at(i));
			 }
			 gameData->rooms = newRooms;
			 curEdit = -1;
			 deleteRoom = false;
		}, .text = "CONFIRM, DELETE ROOM", .pos = { static_cast<int>((windowWidth*renderScale) * 0.4f), static_cast<int>((windowHeight * renderScale) * 0.5f) } };
		confirmBtn.Draw();

		Button cancelBtn{ .onClick = [&] {
			deleteRoom = false;
		}, .text = "Cancel", .pos = { confirmBtn.getEnd().x, static_cast<int>((windowHeight * renderScale) * 0.5f) } };
		cancelBtn.Draw();
		return;
	}

	if (curEdit == -1) DrawPanel();
	else
	{
		auto& editRoom = gameData->rooms.at(curEdit);
		SDL_Color textCol = sdl::WHITE;
		if (editRoom.sprite.size() > 2)
		{
			gameData->mapView->DrawRoom(editRoom.name); // Draw the room
			textCol = sdl::BLACK;
		}
		Button panelBtn{ .onClick = [&] {
			curEdit = -1;
		}, .text = "Editing room: " + editRoom.name, .pos = { 0, 0 } };
		panelBtn.Draw();

		Button delBtn{ .onClick = [&] {
			deleteRoom = true;
		}, .text = "[delete room]", .pos = { panelBtn.getEnd().x, 0} };
		delBtn.Draw();

		Button saveBtn{ .onClick = [&] {
			loader.SaveData();
		}, .text = "[save changes]", .pos = { delBtn.getEnd().x, 0} };
		saveBtn.Draw();

		Button exitBtn{ .onClick = [&] {
			state = GameState::Investigating;
		}, .text = "[exit editor]", .pos = { saveBtn.getEnd().x, 0} };
		exitBtn.Draw();

		if (curEdit == -1) return;

		if (editorFileDrop.size() > 2)
		{
			// Import the sprite and put it onto the screen at this position
			editRoom.props.push_back(Prop{
					.name = editorFileDrop,
					.sprite = editorFileDrop
				});
			editRoom.props.back().pos = SDLWrapper::getMousePos();

			SDLWrapper::LoadSprite(editorFileDrop);
			editorFileDrop.clear(); // We don't need this anymore
		}

		SDLWrapper::DrawString("sprite: " + editRoom.sprite, { 0, 20 }, textCol);
		SDLWrapper::DrawString("components: ", { 0, 36 }, textCol);
		int y = 48;
		for (auto& c : editRoom.components)
		{
			SDLWrapper::DrawString(" - " + c, { 16, y }, textCol);
			y += 16;
		}

		static int _holding = -1;
		static int _typeHolding = -1;
		static gobl::vec2i offsetClick = { 0, 0 };

		if (SDLWrapper::getMouse().bHeld(0) == false && _holding != -1)
		{
			_holding = -1;
			_typeHolding = -1;
		}

		HandleProps<Prop>(editRoom.props, nullptr, editRoom, _holding, offsetClick, _typeHolding, 0);
		HandleProps<Standoff>(editRoom.standOffs, &gameData->suspectSprite, editRoom, _holding, offsetClick, _typeHolding, 2);

		// TODO: Provide tools for editing the existing information
	}
}

void Game::OnFileDropped(std::string dir)
{
	int slash = 0;
	std::string parentDir = "";
	std::string builtDir = "";
	std::string fileName = "";
	for (int i = static_cast<int>(dir.size()) - 1; i > 0; i--)
	{
		if (dir.at(i) == '/' || dir.at(i) == '\\')
		{
			slash = i;
			if (parentDir.size() > 2)
			{
				// std::cout << parentDir << std::endl;
				builtDir += parentDir + '/' + fileName;
				if (parentDir == "sprites")
				{
					break;
				}
				parentDir.clear();
			}
		}
		else if (slash != 0)
		{
			parentDir = dir.at(i) + parentDir;
		}
		else
		{
			fileName = dir.at(i) + fileName;
		}
	}

	//std::cout << builtDir << std::endl;
	editorFileDrop = builtDir;
}