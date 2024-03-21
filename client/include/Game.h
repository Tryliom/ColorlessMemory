#pragma once

#include "Packet.h"
#include "Constants.h"
#include "Gui/Gui.h"
#include "GameManager.h"

#include "ClientNetworkInterface.h"

#include <SFML/Graphics.hpp>

#include <array>

enum class GameState
{
	NONE,
	MAIN_MENU,
	LOBBY,
	GAME
};

class Game
{
 public:
	explicit Game(GameManager& gameManager, ClientNetworkInterface& networkManager, int width, int height);

	void CheckInputs(const sf::Event& event);
	void Update(sf::Time elapsed);
	void SetState(GameState state);
	void Draw(sf::RenderTarget& target);

	void SendPacket(Packet* packet);

 private:
	// Gui
	Gui* _gui { nullptr };
	GameState _state = GameState::NONE;
	sf::RectangleShape _background;

	// Game
	GameManager& _gameManager;
	ClientNetworkInterface& _networkManager;

	int _width = 0;
	int _height = 0;

	void SetBackground(const sf::Texture& texture);

	void OnPacketReceived(Packet& packet);
};