#include "game.hpp"
#include "SDLWrapper.hpp"

template <typename T>
gobl::vec2i DrawCards(std::vector<T>& cards, int& holdIndex, Game::HoldingType& holding, gobl::vec2i pos)
{
	// Ensure T is derived from Card
	static_assert(std::is_base_of<Card, T>::value, "T must be derived from Card");

	const int SPACING = 15 * 4;
	for (int i = 0; i < cards.size(); i++)
	{
		auto item = cards.at(i);
		bool mouseOver = item.mouseOver(pos);

		if (holdIndex == i)
		{
			item.Draw(SDLWrapper::getMousePos() + gobl::vec2<int>{ 0, -16 });

			if (SDLWrapper::getMouse().y > pos.y - 20) pos.x += (SPACING * 2) + 10;
		}
		else
		{
			if (mouseOver) pos.y -= 10;

			item.Draw(pos);

			if (mouseOver)
			{
				if (SDLWrapper::getMouse().bHeld(0) && holdIndex == -1)
				{
					holdIndex = i;
					holding = (Game::HoldingType)item.type;
				}

				pos.y += 20;
				pos.x += SPACING + 10;
			}

			pos.x += SPACING;
			pos.y += i % 2 == 0 ? 2 : -2;
		}
	}

	return pos;
}

void Game::DisplayAccusing()
{
	static bool foundWhat = false;
	static int accusing = -1;

	for (auto& q : questions)
	{
		q.Draw();
		if (q.mouseOver() && SDLWrapper::getMouse().bRelease(0))
		{
			if (q.text == "Who?" && holding == SUSPECT)
			{
				q.answer = suspects.at(holdIndex).name;
				accusing = holdIndex;
			}
			if (q.text == "What?" && holding == WEAPON)
			{
				q.answer = weapons.at(holdIndex).name;
				if (q.answer == weapons.at(weapon).name) foundWhat = true;
				else foundWhat = false;
			}
		}

		if (q.text == "Why?" && accusing != -1) q.answer = (suspects.at(accusing).foundMotive) ? suspects.at(accusing).GetMotive() : "???";
		if (q.text == "Where?") q.answer = mapView->GetMurderRoom();
	}

	gobl::vec2i pos = DrawCards(weapons, holdIndex, holding, { 5, SDLWrapper::getScreenHeight() - (Card::CARD_RECT.y + 3) });
	DrawCards(suspects, holdIndex, holding, pos);

	if (SDLWrapper::getMouse().bRelease(0))
	{
		holdIndex = -1;
		holding = NONE;
	}

	static Button btn = { .onClick = [&]() {
		if ((killer == accusing) && foundWhat) state = Win;
		else
		{
			accusing = -1;
			for (auto& q : questions) q.answer = "";
		}
	}, .text = "Accuse", .pos = questions.back().pos + gobl::vec2<int>{ 0, 50} };
	btn.Draw();
}

void Game::DisplayKiller(bool foundKiller)
{
	gobl::vec2<int> killerPos = { 100, 100 };
	gobl::vec2<int> weaponPos = { 100, 120 + suspectSprite.height };

	int score = 0;

	if (foundKiller)
	{
		suspects.at(killer).Draw(killerPos);
		SDLWrapper::DrawString("The killer was " + suspects.at(killer).name, killerPos + gobl::vec2<int>{ 0, suspectSprite.height }, sdl::BLACK);

		weapons.at(weapon).Draw(weaponPos);
		SDLWrapper::DrawString("with the " + weapons.at(weapon).name, weaponPos + gobl::vec2<int>{ 0, weaponSprite.height }, sdl::BLACK);
		score += 50;
	}
	else SDLWrapper::DrawString("The killer wasn't caught...", killerPos + gobl::vec2<int>{ 0, suspectSprite.height }, sdl::BLACK);

	if (mapView->GetMurderRoom() != "???")
	{
		SDLWrapper::DrawString("they struck in the " + mapView->GetMurderRoom(), weaponPos + gobl::vec2<int>{ 0, weaponSprite.height + 24 }, sdl::BLACK);
		score += 25;
	}
	else SDLWrapper::DrawString("We may never know where...", weaponPos + gobl::vec2<int>{ 0, weaponSprite.height + 24 }, sdl::BLACK);

	if (suspects.at(killer).foundMotive)
	{
		SDLWrapper::DrawString("their motive was " + suspects.at(killer).GetMotive(), weaponPos + gobl::vec2<int>{ 0, weaponSprite.height + 36 }, sdl::BLACK);
		score += 25;
	}
	else SDLWrapper::DrawString("We may never know why...", weaponPos + gobl::vec2<int>{ 0, weaponSprite.height + 36 }, sdl::BLACK);

	std::string grade = "F";
	if (score > 80) grade = "A";
	else if (score > 70) grade = "C";
	else if (score > 0) grade = "D";

	SDLWrapper::DrawString("CASE GRADE: " + grade, gobl::vec2<int>{ SDLWrapper::getScreenWidth() - 220, SDLWrapper::getScreenHeight() - 36 }, sdl::BLACK);
}

void Game::DisplayInterview(float deltaTime)
{
	// FIXME: Move this over to cards
	gobl::vec2<int> speachBubblePos = gobl::vec2<int>{ 10, SDLWrapper::getScreenHeight() >> 1 };
	gobl::vec2<int> suspectPos = gobl::vec2<int>{ 100, speachBubblePos.y - suspectSprite.height };

	suspects.at(interviewing).Draw(suspectPos);
	SDLWrapper::DrawString(suspects.at(interviewing).name, suspectPos + gobl::vec2<int>{ 0, suspectSprite.height }, sdl::BLACK);
	speachBubblePos.y += 25;
	SDLWrapper::DrawString("How can I help you detective?", speachBubblePos, sdl::WHITE);
	speachBubblePos.y += 25;

	static std::vector<std::string> answers{
		"Why would you have killed Mr. Boddy?", "Nevermind..."
	};
	static QuestionObject responseBox = { .text = "Response ", .pos = gobl::vec2<int>{speachBubblePos.x + 100, speachBubblePos.y + 25} };
	responseBox.Draw();

	if (responseBox.answer.size() < 2)
	{
		gobl::vec2<int> pos = { 5, SDLWrapper::getScreenHeight() - 64 };
		const int SPACING = 15 * 4;

		for (int i = 0; i < answers.size(); i++)
		{
			auto item = answers.at(i);
			bool mouseOver = SDLWrapper::getMouse().x < pos.x + SPACING + 12 && SDLWrapper::getMouse().x > pos.x && SDLWrapper::getMouse().y < pos.y + 64 && SDLWrapper::getMouse().y > pos.y;

			if (holdIndex == i) // FIXME: Make some cards for these guys
			{
				//sdl::CardRect(SDLWrapper::getMousePos(), { SPACING + 12, 64 }, sdl::DARK_GREY);
				SDLWrapper::DrawString(item, SDLWrapper::getMousePos() + gobl::vec2<int>{ 0, 16 }, sdl::BLACK);

				if (SDLWrapper::getMouse().y > pos.y - 20) pos.x += (SPACING * 2) + 10;
			}
			else
			{
				if (mouseOver) pos.y -= 10;

				//sdl::CardRect(pos, { SPACING + 12, 64 }, sdl::DARK_GREY);
				SDLWrapper::DrawString(item, pos + gobl::vec2<int>{ 0, 16 }, sdl::BLACK);

				if (mouseOver)
				{
					if (SDLWrapper::getMouse().bHeld(0) && holdIndex == -1)
					{
						holdIndex = i;
					}

					pos.y += 10;
					pos.x += SPACING + 10;
				}

				pos.x += SPACING;
			}
		}

		if (SDLWrapper::getMouse().bRelease(0))
		{
			if (responseBox.mouseOver()) responseBox.answer = answers.at(holdIndex);
			holdIndex = -1;
		}
	}
	else
	{
		if (responseBox.answer == answers.back())
		{
			state = Investigating;
			responseBox.answer = "";
			return;
		}

		SDLWrapper::DrawString("I suppose it would be " + suspects.at(interviewing).GetMotive(), speachBubblePos, sdl::BLACK); // FIXME: Do an actual minigame with the interview
		suspects.at(interviewing).foundMotive = true;

		static Button jaccuse = { .onClick = [&]() {
			state = Accusing;
			responseBox.answer = "";
		}, .text = "Accuse", .pos = { SDLWrapper::getScreenWidth() - 100, SDLWrapper::getScreenHeight() - 50} };
		jaccuse.Draw();

		static Button btn = { .onClick = [&]() {
			state = Investigating;
			responseBox.answer = "";
		}, .text = "Done", .pos = { SDLWrapper::getScreenWidth() - 100, SDLWrapper::getScreenHeight() - 25} };
		btn.Draw();
	}
}

void Game::DisplayIntroduction(float deltaTime)
{
	SDLWrapper::DrawSprite(introScene.background);

	const int FONT_SIZE = 32;
	gobl::vec2i introLinePos = { 100, SDLWrapper::getScreenHeight() >> 1 };
	SDLWrapper::DrawString(introScene.line, introLinePos + gobl::vec2i{ -2, 2 }, sdl::DARK_GREY, FONT_SIZE); // Shadow
	SDLWrapper::DrawString(introScene.line, introLinePos, sdl::WHITE, FONT_SIZE);

	static QuestionObject continueFactor = { .text = "Response ", .pos = {100, (SDLWrapper::getScreenHeight() >> 1) + 50} };
	continueFactor.Draw();

	if (continueFactor.answer.size() < 2)
	{
		DrawCards(introScene.response, holdIndex, holding, { 5, SDLWrapper::getScreenHeight() - (Card::CARD_RECT.y + 3) });

		if (SDLWrapper::getMouse().bRelease(0) && holdIndex != -1)
		{
			if (continueFactor.mouseOver()) continueFactor.answer = introScene.response.at(holdIndex).name;
			holdIndex = -1;
		}
	}
	else
	{
		const float MAX_TIME = 5.0f;
		static float time = 0.0f;
		time += deltaTime;
		SDLWrapper::DrawString("Arriving to crime scene in " + std::to_string(int(ceil(MAX_TIME - time))) + "...", gobl::vec2<int>{ 10, SDLWrapper::getScreenHeight() - 16 }, sdl::BLACK);
		if (time >= MAX_TIME) state = Investigating;
	}
}

bool Game::OnUserUpdate(float deltaTime)
{
	// SDLWrapper::DrawString(std::to_string(deltaTime), { 0, 8 }, sdl::WHITE);

	if (SDLWrapper::getKeyboard().bDown(SDLK_TAB))
	{
		if (state == Investigating) state = Accusing;
		else if (state == Accusing) state = Investigating;
	}

	if (state == Investigating)
	{
		holdIndex = -1;
		interviewing = 0;
	}

	int stateEvent = 0;

	switch (state)
	{
	case Game::Introduction:
		DisplayIntroduction(deltaTime);
		break;
	case Game::Interviewing:
		DisplayInterview(deltaTime);
		break;
	case Game::Accusing:
		DisplayAccusing();
		break;
	case Game::Investigating:
		stateEvent = mapView->Display(deltaTime);
		if (stateEvent == -1) state = Lose;
		else if (stateEvent > 0)
		{
			interviewing = stateEvent - 1;
			state = Interviewing;
		}
		break;
	case Game::Win:
		DisplayKiller(true);
		break;
	case Game::Lose:
		DisplayKiller(false);
		break;
	default:
		break;
	}

	return true;
}

Game::Game()
{
	std::vector<Room> rooms{};
	LoadData(rooms);

	std::cout << "Shuffling..." << std::endl;
	srand(static_cast<unsigned int>(time(NULL)));
	killer = rand() % suspects.size();
	weapon = rand() % weapons.size();

	suspects.at(killer).isKiller = true;

	std::cout << "Picked killer and weapon. Begin game!" << std::endl;

	mapView = new MapView(suspects, rooms);
	mapView->weapon = weapons.at(weapon).name;
}