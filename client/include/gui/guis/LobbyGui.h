#pragma once

#include "gui/Gui.h"
#include "DeckIcon.h"
#include "PlayerUi.h"

#include <array>

class LobbyGui final : public Gui
{
private:
	PlayerUi _player1;
	PlayerUi _player2;

	std::array<DeckIcon, 5> _deckIcons;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit LobbyGui();

	void OnPacketReceived(const Packet& packet) override;
};