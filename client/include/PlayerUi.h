#pragma once

#include "gui/Text.h"

#include <SFML/Graphics.hpp>

class PlayerUi :
	public sf::Drawable
{
 private:
	sf::RectangleShape _playerIcon;
	sf::RectangleShape _playerIconShadow;
	sf::RectangleShape _playerBackground;
	sf::RectangleShape _playerBackgroundShadow;

	Text _playerName;
	sf::Vector2f _playerNamePosition;
	Text _playerScore;
	sf::Vector2f _playerScorePosition;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 public:
	PlayerUi() = default;
	PlayerUi(bool isPlayer1, sf::Vector2f position, bool displayScore);

	void SetName(const std::string& name);
	void SetScore(int score);
	void SetIcon(std::size_t iconIndex);
};