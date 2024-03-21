#include "AssetManager.h"
#include "Game.h"
#include "MyPackets.h"

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();
	Game::Initialize();

	sf::RenderWindow _window(sf::RenderWindow(sf::VideoMode(Game::WIDTH, Game::HEIGHT), "Colorless Memory", sf::Style::Default));

	sf::Clock clock;

	while (_window.isOpen())
	{
		sf::Event event{};

		while (_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				_window.close();
				break;
			}

			Game::CheckInputs(event);
		}

		Game::Update(clock.restart());

		_window.clear();

		Game::Render(_window);

		_window.display();
	}

	_client.socket->disconnect();
	_networkClientManager.Stop();

	return EXIT_SUCCESS;
}