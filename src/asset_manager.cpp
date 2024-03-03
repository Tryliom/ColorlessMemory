#include "asset_manager.h"

#include <array>

namespace AssetManager
{
	static std::array<sf::Texture, static_cast<int>(TextureType::END)> textures;
	static sf::Font mainFont;

	void Initialize()
	{
		textures[static_cast<int>(TextureType::BACKGROUND)].loadFromFile("data/textures/background.png");

		mainFont.loadFromFile("data/font/Retro Gaming.ttf");
	}

	sf::Font& GetMainFont()
	{
		return mainFont;
	}

	sf::Texture& GetTexture(const TextureType type)
	{
		return textures[static_cast<int>(type)];
	}
}
