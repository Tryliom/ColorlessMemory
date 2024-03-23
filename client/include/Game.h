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
	void Update(sf::Time elapsed, sf::Vector2f mousePosition);
	void SetState(GameState state);
	void Draw(sf::RenderTarget& target);

	void SendPacket(Packet* packet);
	void Quit();

	void OnQuit(std::function<void()> onQuit);

 private:
	sf::RectangleShape _background;
	std::function<void()> _onQuit;
	// Gui
	Gui* _gui { nullptr };
	// Game
	GameManager& _gameManager;
	ClientNetworkInterface& _networkManager;

	GameState _state = GameState::NONE;

	int _width = 0;
	int _height = 0;

	void SetBackground(const sf::Texture& texture);
	void OnPacketReceived(Packet& packet);
};