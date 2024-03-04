#include "Game.h"

#include "NetworkClientManager.h"
#include "PacketManager.h"
#include "Logger.h"

#include "AssetManager.h"
#include "gui/guis/MenuGui.h"
#include "gui/guis/LobbyGui.h"

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

	bool onPacketReceived(const Packet& packet);
	void setBackground(const sf::Texture& texture);

	void Initialize()
	{
		_window.setFramerateLimit(165);
		_window.setVerticalSyncEnabled(true);

		// Set the size of the game
		Game::HEIGHT = static_cast<float>(_window.getSize().y);
		Game::WIDTH = static_cast<float>(_window.getSize().x);

		setBackground(AssetManager::GetTexture(TextureType::BACKGROUND_MENU));
		SetState(GameState::MAIN_MENU);

		// Network
		sf::Socket::Status status = _client.socket->connect(HOST_NAME, PORT);

		if (status != sf::Socket::Done)
		{
			LOG_ERROR("Could not connect to server");
			_window.close();
		}

		_networkClientManager.SetOnMessageReceived(Game::onPacketReceived);
		_networkClientManager.StartThreads(_client);
	}

	bool onPacketReceived(const Packet& packet)
	{
		if (packet.type == PacketType::Message)
		{
			MessagePacket messageReceived = dynamic_cast<const MessagePacket&>(packet);
			LOG(messageReceived.playerName + ": " + messageReceived.message);
		}

		return true;
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

	void setBackground(const sf::Texture& texture)
	{
		_background.setTexture(&texture);
		_background.setSize(sf::Vector2f(texture.getSize()));
		_background.setPosition(0, 0);
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

		if (state == GameState::LOBBY)
		{
			_gui = new LobbyGui();
		}

		_state = state;
	}

	void Exit()
	{
		_window.close();
	}
}