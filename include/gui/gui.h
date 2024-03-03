#pragma once

#include "button.h"
#include "text.h"

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

public:
	virtual void Update(sf::Time elapsed);
	virtual void CheckInputs(sf::Event event);
};
