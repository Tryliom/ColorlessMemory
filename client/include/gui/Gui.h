#pragma once

#include "Button.h"
#include "Text.h"

#include <SFML/Graphics.hpp>

class Gui : public sf::Drawable
{
public:
	Gui();

protected:
	sf::RectangleShape _background;
	std::vector<Button> _buttons;
	std::vector<Text> _texts;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	virtual void OnUpdate(sf::Time elapsed) {}

public:
	void Update(sf::Time elapsed);
	void CheckInputs(sf::Event event);
};
