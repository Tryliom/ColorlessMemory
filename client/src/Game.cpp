#include "Game.h"

#include "NetworkClientManager.h"
#include "PacketManager.h"
#include "Logger.h"

#include "AssetManager.h"
#include "gui/guis/MenuGui.h"
#include "gui/guis/LobbyGui.h"
#include "gui/guis/GameGui.h"


#include <SFML/Graphics.hpp>

namespace Game
{
	// Network
	Client _client;
	NetworkClientManager _networkClientManager;

	// Game display and physics
	sf::RenderWindow _window(sf::RenderWindow(sf::VideoMode(Game::WIDTH, Game::HEIGHT), "Colorless Memory", sf::Style::Default));

	// Gui
	Gui* _gui { nullptr };
	GameState _state = GameState::NONE;
	sf::RectangleShape _background;

	// Game data
	LobbyData _lobby;
	GameData _game;

	void OnPacketReceived(const Packet& packet);
	void SetBackground(const sf::Texture& texture);

	void Initialize()
	{
		_window.setFramerateLimit(165);
		_window.setVerticalSyncEnabled(true);

		// Set the size of the game
		Game::HEIGHT = static_cast<float>(_window.getSize().y);
		Game::WIDTH = static_cast<float>(_window.getSize().x);

		SetBackground(AssetManager::GetTexture(TextureType::BACKGROUND_MENU));
		SetState(GameState::MAIN_MENU);

		// Network
		sf::Socket::Status status = _client.socket->connect(HOST_NAME, PORT);

		if (status != sf::Socket::Done)
		{
			LOG_ERROR("Could not connect to server");
			_window.close();
		}

		//TODO: Get the name of the user with the filesystem and send it to the server

		_networkClientManager.StartThreads(_client);
	}

	void OnPacketReceived(const Packet& packet)
	{
		if (packet.type == PacketType::JoinLobby)
		{
			auto& joinLobbyPacket = dynamic_cast<const JoinLobbyPacket&>(packet);

			_lobby.IsHost = joinLobbyPacket.IsHost;
			_lobby.WaitingForOpponent = joinLobbyPacket.WaitingForOpponent;
			_game.Reset();
		}
		else if (packet.type == PacketType::StartGame)
		{
			auto& startGamePacket = dynamic_cast<const StartGamePacket&>(packet);

			_game.YourTurn = startGamePacket.YourTurn;
			_game.ReadyToStart = true;

			SetState(GameState::GAME);
		}
	}

	void Update(sf::Time elapsed)
	{
		if (_gui != nullptr)
		{
			_gui->Update(elapsed);
		}

		while (Packet* packet = _networkClientManager.PopPacket())
		{
			OnPacketReceived(*packet);

			auto packetTypeValue = static_cast<int>(packet->type);

			if (packetTypeValue >= 0 && packetTypeValue <= static_cast<int>(PacketType::Invalid))
			{
				delete packet;
			}
		}
	}

	void CheckInputs(sf::Event event)
	{
		if (event.type == sf::Event::Closed)
		{
			_window.close();
			return;
		}

		if (_gui != nullptr)
		{
			_gui->CheckInputs(event);
		}
	}

	void Render()
	{
		_window.clear();

		// Render background
		_window.draw(_background);

		if (_gui != nullptr)
		{
			_window.draw(*_gui);
		}

		_window.display();
	}

	void SetBackground(const sf::Texture& texture)
	{
		_background.setTexture(&texture);
		_background.setSize(sf::Vector2f(texture.getSize()));
		_background.setPosition(0, 0);
	}

	int StartLoop()
	{
		sf::Clock clock;

		while (_window.isOpen())
		{
			sf::Event event{};

			while (_window.pollEvent(event))
			{
				CheckInputs(event);
			}

			Update(clock.restart());
			Render();
		}

		_client.socket->disconnect();
		_networkClientManager.Stop();

		return EXIT_SUCCESS;
	}

	sf::RenderWindow& GetWindow() { return _window; }

	void SetState(GameState state)
	{
		if (_state == state) return;

		delete _gui;

		if (state == GameState::MAIN_MENU)
		{
			_gui = new MenuGui();
		}

		if (state == GameState::LOBBY)
		{
			_gui = new LobbyGui();
			_lobby.IsHost = true;
			_lobby.WaitingForOpponent = true;
			SendPacket(new JoinLobbyPacket());
		}

		if (state == GameState::GAME)
		{
			_gui = new GameGui();
		}

		_state = state;
	}

	void Exit()
	{
		_window.close();
	}

	void SendPacket(Packet* packet)
	{
		_client.SendPacket(packet);
	}

	LobbyData& GetLobby()
	{
		return _lobby;
	}

	GameData& GetGame()
	{
		return _game;
	}
}