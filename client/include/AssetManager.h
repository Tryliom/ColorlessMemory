#pragma once

#include "DeckType.h"

#include <SFML/Graphics.hpp>

enum class TextureType
{
	BACKGROUND_MENU,
    SIMPLE_ICON_BACKGROUND,
    PLAYER1_ICON_BACKGROUND,
    PLAYER2_ICON_BACKGROUND,
	END
};

constexpr std::size_t CARD_ICON_COUNT = 25;

namespace AssetManager
{
	void Initialize();

	sf::Font& GetMainFont();
	sf::Texture& GetTexture(TextureType type);
	sf::Texture& GetCardTexture(DeckType type, bool isRevealed = false);
	sf::Texture& GetCardIcon(std::size_t index);
}