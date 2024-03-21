#include "Game.h"

#include "NetworkClientManager.h"
#include "PacketManager.h"
#include "Logger.h"

#include "AssetManager.h"
#include "gui/guis/MenuGui.h"
#include "gui/guis/LobbyGui.h"
#include "gui/guis/GameGui.h"

#include <SFML/Graphics.hpp>

#include <cstdlib>

//TODO: Convert into a class
//TODO: Use a render target instead of a window
namespace Game
{
	// Network
	Client _client;
	NetworkClientManager _networkClientManager;

	// Gui
	Gui* _gui { nullptr };
	GameState _state = GameState::NONE;
	sf::RectangleShape _background;

	// Game data
	PlayerData _player;
	Lobby _lobby;
	GameData _game;

	void OnPacketReceived(const Packet& packet);
	void SetBackground(const sf::Texture& texture);

	void Initialize()
	{
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

		// Get player name
		std::string username = std::getenv("USERNAME") ? std::getenv("USERNAME") : "default";
		if (!username.empty()) {
			username[0] = std::toupper(username[0]);
		}
		_player.Name = username;

		_networkClientManager.StartThreads(_client);
	}

	void OnPacketReceived(const Packet& packet)
	{
		if (packet.Type == PacketType::LobbyInformation)
		{
			auto& joinLobbyPacket = dynamic_cast<const LobbyInformationPacket&>(packet);

			_lobby.IsHost = joinLobbyPacket.IsHost;
			_lobby.WaitingForOpponent = joinLobbyPacket.WaitingForOpponent;
			_lobby.Player1.Name = joinLobbyPacket.Player1Name;
			_lobby.Player1.IconIndex = joinLobbyPacket.Player1Icon;
			_lobby.Player2.Name = joinLobbyPacket.Player2Name;
			_lobby.Player2.IconIndex = joinLobbyPacket.Player2Icon;
		}
		else if (packet.Type == PacketType::StartGame)
		{
			auto& startGamePacket = dynamic_cast<const StartGamePacket&>(packet);

			_lobby.DeckType = startGamePacket.ChosenDeckType;
			_game.Reset(_lobby);
			_game.YourTurn = startGamePacket.YourTurn;
		}
		else if (packet.Type == PacketType::Turn)
		{
			auto& turnPacket = dynamic_cast<const TurnPacket&>(packet);

			_game.YourTurn = turnPacket.YourTurn;
		}

		if (_gui != nullptr)
		{
			_gui->OnPacketReceived(packet);
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

			auto packetTypeValue = static_cast<int>(packet->Type);

			if (packetTypeValue >= 0 && packetTypeValue <= static_cast<int>(PacketType::Invalid))
			{
				delete packet;
			}
		}
	}

	void CheckInputs(sf::Event event)
	{
		if (_gui != nullptr)
		{
			_gui->CheckInputs(event);
		}
	}

	void Render(sf::RenderTarget& target)
	{
		// Render background
		target.draw(_background);

		if (_gui != nullptr)
		{
			target.draw(*_gui);
		}
	}

	void SetBackground(const sf::Texture& texture)
	{
		_background.setTexture(&texture);
		_background.setSize(sf::Vector2f(texture.getSize()));
		_background.setPosition(0, 0);
	}

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
			SendPacket(new JoinLobbyPacket(_player.Name, _player.IconIndex));
		}

		if (state == GameState::GAME)
		{
			_gui = new GameGui();
		}

		_state = state;
	}

	void SendPacket(Packet* packet)
	{
		_client.SendPacket(packet);
	}

	Lobby& GetLobby()
	{
		return _lobby;
	}

	GameData& GetGame()
	{
		return _game;
	}

	PlayerData& GetPlayer()
	{
		return _player;
	}
}