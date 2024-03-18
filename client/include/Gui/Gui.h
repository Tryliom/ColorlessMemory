#pragma once

#include "Button.h"
#include "Text.h"
#include "Packet.h"

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
	/**
	 * @brief Draw additional elements after background and before buttons and texts
	 */
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const {}
	virtual void OnCheckInputs(sf::Event event) {}

public:
	void Update(sf::Time elapsed);
	void CheckInputs(sf::Event event);
	virtual void OnPacketReceived(const Packet& packet) {}
};
