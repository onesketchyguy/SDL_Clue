#include "questionObject.hpp"

void QuestionObject::Draw()
{
	gobl::vec2i textOffset = { ((int32_t)text.size() * 6) + 2 , 3 };
	rect = { ((int32_t)text.size() * 8), 24 };

	bool hovered = mouseOver();
	SDLWrapper::DrawRect(pos.x, pos.y - textOffset.y, rect.x, rect.y, hovered ? sdl::GREY : sdl::WHITE);
	SDLWrapper::OutlineRect(pos.x, pos.y - textOffset.y, rect.x, rect.y, hovered ? sdl::BLACK : sdl::DARK_GREY);
	SDLWrapper::DrawString(text, { pos.x + 2, pos.y }, sdl::BLACK);

	gobl::vec2i cardSlotPos = { pos.x, pos.y + rect.y - 4 };
	if (card != nullptr)
	{
		card->Draw(cardSlotPos);
	}
	else
	{
		// Draw the actual card slot
		SDLWrapper::DrawRect(cardSlotPos.x, cardSlotPos.y, Card::CARD_RECT.x, Card::CARD_RECT.y, sdl::GREY);
		SDLWrapper::OutlineRect(cardSlotPos.x, cardSlotPos.y, Card::CARD_RECT.x, Card::CARD_RECT.y, sdl::BLACK);
		SDLWrapper::OutlineRect(cardSlotPos.x + 1, cardSlotPos.y + 1, Card::CARD_RECT.x - 2, Card::CARD_RECT.y - 2, sdl::DARK_GREY);
		SDLWrapper::DrawRect(cardSlotPos.x + 10, cardSlotPos.y + 10, Card::CARD_RECT.x - 20, Card::CARD_RECT.y - 20, sdl::LIGHT_GREY);
	}
}

bool QuestionObject::mouseOver()
{
	// Check if the mouse is over the question object
	gobl::vec2i scale = { Card::CARD_RECT.x, rect.y + Card::CARD_RECT.y };
	return SDLWrapper::getMouse().over(pos, scale);
}