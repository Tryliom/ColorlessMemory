#include "game.h"

#include "gui/guis/menu_gui.h"
#include "asset_manager.h"

#include "network_client_manager.h"
#include "packet_manager.h"
#include "logger.h"

#include <SFML/Graphics.hpp>

namespace Game
{
	// Network
	Client _client;
	NetworkClientManager _networkClientManager;

	// Game display and physics
	sf::RenderWindow _window(sf::RenderWindow(sf::VideoMode(1920, 1080), "Colorless Memory", sf::Style::Default));

	// Gui
	Gui* _gui { nullptr };
	GameState _state = GameState::NONE;
	sf::RectangleShape _background;

	void Initialize()
	{
		_window.setFramerateLimit(165);
		_window.setVerticalSyncEnabled(true);

		// Set the size of the game
		Game::HEIGHT = static_cast<float>(_window.getSize().y);
		Game::WIDTH = static_cast<float>(_window.getSize().x);

		auto& backgroundTexture = AssetManager::GetTexture(TextureType::BACKGROUND);

		_background.setTexture(&backgroundTexture);
		_background.setSize(sf::Vector2f(backgroundTexture.getSize()));
		_background.setPosition(0, 0);

		SetState(GameState::MAIN_MENU);

		// Network
		sf::Socket::Status status = _client.socket->connect(HOST_NAME, PORT);

		if (status != sf::Socket::Done)
		{
			LOG_ERROR("Could not connect to server");
			_window.close();
		}

		_networkClientManager.SetOnMessageReceived([](sf::Packet& packet, PacketType packetType)
		{
			if (packetType == PacketType::Message)
			{
				MessagePacket messageReceived = PacketManager::GetMessagePacket(packet);
				LOG(messageReceived.playerName + ": " + messageReceived.message);
			}

			return true;
		});

		_networkClientManager.StartThreads(_client);
	}

	void update(sf::Time elapsed)
	{
		if (_gui != nullptr)
		{
			_gui->Update(elapsed);
		}
	}


	void checkInputs(sf::Event event)
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

	void render()
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

	int Loop()
	{
		sf::Clock clock;

		while (_window.isOpen())
		{
			sf::Event event{};

			while (_window.pollEvent(event))
			{
				checkInputs(event);
			}

			update(clock.restart());
			render();
		}

		return EXIT_SUCCESS;
	}

	sf::RenderWindow& GetWindow() { return _window; }

	void SetState(GameState state)
	{
		if (_state == state)
		{
			return;
		}

		if (state == GameState::MAIN_MENU)
		{
			_gui = new MenuGui();
		}

		_state = state;
	}

	void Exit()
	{
		_window.close();
	}
}