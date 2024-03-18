#pragma once

struct CardIconIndex final
{
	char Index{};
};

struct CardIndex final
{
	char Index{};
};

static constexpr auto UNKNOWN_ICON_INDEX = CardIconIndex{ -1 };
static constexpr auto UNKNOWN_CARD_INDEX = CardIndex{ -1 };

enum class IconType : char
{
	Icon1, Icon2, Icon3, Icon4, Icon5
};

enum class DeckType : char
{
	Deck3x2, Deck7x2, Deck6x5, Deck7x6, Deck10x5
};