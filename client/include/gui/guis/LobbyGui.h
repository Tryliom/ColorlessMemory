#pragma once

#include "gui/Gui.h"
#include "DeckIcon.h"

#include <array>

class LobbyGui final : public Gui
{
private:
	sf::RectangleShape _player1Icon;
	sf::RectangleShape _player2Icon;
	sf::RectangleShape _player1IconShadow;
	sf::RectangleShape _player2IconShadow;
	sf::RectangleShape _player1Background;
	sf::RectangleShape _player2Background;
	sf::RectangleShape _player1BackgroundShadow;
	sf::RectangleShape _player2BackgroundShadow;

	std::array<DeckIcon, 5> _deckIcons;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit LobbyGui();

	void OnPacketReceived(const Packet& packet) override;
};