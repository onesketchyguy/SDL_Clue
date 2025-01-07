#include "questionObject.hpp"

void QuestionObject::Draw()
{
	gobl::vec2<int> repos = { ((int32_t)text.size() * 8) + 2 , 3 };
	gobl::vec2<int> rect = { ((int32_t)text.size() * 8) + 16 + ((int32_t)answer.size() * 8) + 4, 12 };

	bool hovered = MouseOver();
	if (hovered) SDLWrapper::DrawRect(pos.x - repos.x, pos.y - repos.y, rect.x, rect.y, sdl::DARK_GREY);
	else SDLWrapper::OutlineRect(pos.x - repos.x, pos.y - repos.y, rect.x, rect.y, sdl::WHITE);
	SDLWrapper::DrawString(text, { pos.x - (repos.x - 2), pos.y }, sdl::WHITE);
	SDLWrapper::DrawString(answer, { pos.x + 8, pos.y }, sdl::WHITE);
	SDLWrapper::DrawLine({ pos.x + 6.0f, pos.y + 8.0f }, { pos.x + (8.0f + (float)answer.size()), pos.y + 8.0f }, sdl::WHITE);
}

bool QuestionObject::MouseOver()
{
	// Check if the mouse is over the question object
	gobl::vec2<int> rect = { ((int32_t)text.size() * 8) + 12 + ((int32_t)answer.size() * 8), 8 };
	return SDLWrapper::getMouse().x < pos.x + rect.x && SDLWrapper::getMouse().x > pos.x - (text.size() * 6) && SDLWrapper::getMouse().y < pos.y + rect.y && SDLWrapper::getMouse().y > pos.y;
}