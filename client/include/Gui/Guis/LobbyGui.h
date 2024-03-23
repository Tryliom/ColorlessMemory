#pragma once

#include "gui/Gui.h"
#include "DeckIcon.h"
#include "PlayerUi.h"

#include <array>

class Game;
class GameManager;

class LobbyGui final : public Gui
{
private:
	Game& _game;
	GameManager& _gameManager;

	float _height = 0;
	float _width = 0;

	PlayerUi _player1;
	PlayerUi _player2;

	std::array<DeckIcon, 5> _deckIcons;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit LobbyGui(Game& game, GameManager& gameManager, float width, float height);

	void OnPacketReceived(Packet& packet) override;
};