#pragma once

#include "gui/Gui.h"

class LobbyGui final : public Gui
{
private:
	sf::RectangleShape _player1Icon;
	sf::RectangleShape _player2Icon;
	sf::RectangleShape _player1Background;
	sf::RectangleShape _player2Background;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit LobbyGui();

	void OnPacketReceived(const Packet& packet) override;
};