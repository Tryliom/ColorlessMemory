#pragma once

#include "gui/Gui.h"
#include "Constants.h"

#include <array>

class Game;
class GameManager;

class MenuGui final : public Gui
{
private:
	std::array<sf::RectangleShape, 5> _icons;
	std::array<sf::RectangleShape, 5> _iconShadows;
	std::array<sf::RectangleShape, 5> _backgrounds;
	std::array<sf::RectangleShape, 5> _backgroundShadows;
	std::array<IconType, 5> _iconIndexes = { IconType::Icon1, IconType::Icon2, IconType::Icon3, IconType::Icon4, IconType::Icon5 };

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit MenuGui(Game& game, GameManager& gameManager, float width, float height);
};