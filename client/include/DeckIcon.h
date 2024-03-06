#pragma once

#include "AssetManager.h"

#include <SFML/Graphics.hpp>

class DeckIcon
{
 private:
	sf::RectangleShape _card1;
	sf::RectangleShape _card2;

 public:
	DeckIcon() = default;

	void SetTexture(CardType cardType);
	void SetPosition(sf::Vector2f position);

	void Draw(sf::RenderTarget& target, sf::RenderStates states) const;
};