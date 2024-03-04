#pragma once

#include <SFML/Graphics.hpp>

enum class TextureType
{
	BACKGROUND_MENU,
	END
};

enum class CardType
{
	Common, Rare, Epic, Legendary, Unique, END
};

constexpr std::size_t CARD_ICON_COUNT = 25;

namespace AssetManager
{
	void Initialize();

	sf::Font& GetMainFont();
	sf::Texture& GetTexture(TextureType type);
	sf::Texture& GetCardTexture(CardType type, bool isFlipped = false);
	sf::Texture& GetCardIcon(std::size_t index);
}