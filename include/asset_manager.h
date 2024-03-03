#pragma once

#include <SFML/Graphics.hpp>

enum class TextureType
{
	BACKGROUND,
	END
};

namespace AssetManager
{
	void Initialize();

	sf::Font& GetMainFont();
	sf::Texture& GetTexture(TextureType type);
}