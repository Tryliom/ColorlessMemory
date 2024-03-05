#pragma once

#include "gui/Gui.h"

#include <array>

class MenuGui final : public Gui
{
private:
	std::array<sf::RectangleShape, 5> _icons;
	std::array<sf::RectangleShape, 5> _backgrounds;
	std::array<std::size_t, 5> _iconIndexes = { 20, 21, 22, 23, 24 };

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit MenuGui();
};