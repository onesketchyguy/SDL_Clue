#include "questionObject.hpp"

void QuestionObject::Draw()
{
	gobl::vec2<int> repos = { ((int32_t)text.size() * 8) + 2 , 3 };
	rect = { ((int32_t)text.size() * 8) + 20 + ((int32_t)answer.size() * 8) + 4, 24 };

	bool hovered = mouseOver();
	SDLWrapper::DrawRect(pos.x - repos.x, pos.y - repos.y, rect.x, rect.y, hovered ? sdl::GREY : sdl::WHITE);
	SDLWrapper::OutlineRect(pos.x - repos.x, pos.y - repos.y, rect.x, rect.y, hovered ? sdl::BLACK : sdl::DARK_GREY);
	SDLWrapper::DrawString(text, { pos.x - (repos.x - 2), pos.y }, sdl::BLACK);
	SDLWrapper::DrawString(answer, { pos.x + 16, pos.y }, sdl::BLACK);
	SDLWrapper::DrawLine({ pos.x + 6.0f, pos.y + 8.0f }, { pos.x + (8.0f + (float)answer.size()), pos.y + 8.0f }, sdl::BLACK);
}

bool QuestionObject::mouseOver()
{
	// Check if the mouse is over the question object
	return SDLWrapper::getMouse().x < pos.x + rect.x && SDLWrapper::getMouse().x > pos.x - (text.size() * 6) && SDLWrapper::getMouse().y < pos.y + rect.y && SDLWrapper::getMouse().y > pos.y;
}