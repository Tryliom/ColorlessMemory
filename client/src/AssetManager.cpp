#include "AssetManager.h"

#include <array>

namespace AssetManager
{
	static std::array<sf::Texture, static_cast<int>(TextureType::END)> textures;
	static sf::Font mainFont;
	static std::array<sf::Texture, static_cast<int>(CardType::END) * 2> cardTextures;
	static std::array<sf::Texture, CARD_ICON_COUNT> cardIcons;

	void Initialize()
	{
		textures[static_cast<int>(TextureType::BACKGROUND_MENU)].loadFromFile("data/textures/background.png");
        textures[static_cast<int>(TextureType::SIMPLE_ICON_BACKGROUND)].loadFromFile("data/textures/ui/0.png");
        textures[static_cast<int>(TextureType::PLAYER1_ICON_BACKGROUND)].loadFromFile("data/textures/ui/1.png");
        textures[static_cast<int>(TextureType::PLAYER2_ICON_BACKGROUND)].loadFromFile("data/textures/ui/2.png");

		mainFont.loadFromFile("data/font/Retro Gaming.ttf");

		for (auto i = 0; i < static_cast<int>(CardType::END) * 2; i++)
		{
			cardTextures[i].loadFromFile("data/textures/cards/0" + std::to_string(i) + ".png");
		}

		for (auto i = 0; i < CARD_ICON_COUNT; i++)
		{
			auto str = i < 10 ? "0" + std::to_string(i) : std::to_string(i);
			cardIcons[i].loadFromFile("data/textures/icons/" + str + ".png");
		}
	}

	sf::Font& GetMainFont()
	{
		return mainFont;
	}

	sf::Texture& GetTexture(const TextureType type)
	{
		return textures[static_cast<int>(type)];
	}

	sf::Texture& GetCardTexture(const CardType type, const bool isFlipped)
	{
		return cardTextures[static_cast<int>(type) * 2 + (isFlipped ? 1 : 0)];
	}

	sf::Texture& GetCardIcon(const std::size_t index)
	{
		return cardIcons[index];
	}
}
