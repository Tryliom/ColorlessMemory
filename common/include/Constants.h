#pragma once

enum class IconType : char
{
	Icon1, Icon2, Icon3, Icon4, Icon5
};

struct CardIconIndex final
{
	char Index{};

	bool operator==(const CardIconIndex& other) const
	{
		return Index == other.Index;
	}
};

struct CardIndex final
{
	char Index{};

	bool operator==(const CardIndex& other) const
	{
		return Index == other.Index;
	}
};

static constexpr auto UNKNOWN_ICON_INDEX = CardIconIndex{ -1 };
static constexpr auto DEFAULT_ICON_INDEX = CardIconIndex{ 0 };
static constexpr auto START_PLAYER_ICON_INDEX = CardIconIndex{ 20 };
static constexpr auto UNKNOWN_CARD_INDEX = CardIndex{ -1 };


enum class DeckType : char
{
	Deck3x2, Deck7x2, Deck6x5, Deck7x6, Deck10x5
};