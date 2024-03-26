#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"

inline static float HEIGHT = 1080.f;
inline static float WIDTH = 1920.f;

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();

	NetworkServerManager networkServerManager(PORT);
	Server server(networkServerManager);

	// Network
	NetworkClientManager networkClientManager(HOST_NAME, PORT);

	// Set the size of the game
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Colorless Memory", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	std::array<GameManager, 2> gameManagers = {
		GameManager(),
		GameManager()
	};
	std::array<Game, 2> games = {
		Game(gameManagers[0], networkClientManager, WIDTH, HEIGHT),
		Game(gameManagers[1], networkClientManager, WIDTH, HEIGHT)
	};

	for (auto& game : games)
	{
		game.OnQuit([&]() {
			window.close();
		});
	}

	//TODO: Ask for username and save it to file
	// Get player name
	std::string username = std::getenv("USERNAME") ? std::getenv("USERNAME") : "default";
	if (!username.empty()) {
		username[0] = std::toupper(username[0]);
	}

	for (auto& gameManager : gameManagers)
	{
		gameManager.SetUsername(username);
	}

	sf::Clock clock;
	int gameIndex = 0;
	auto gameNumber = Text(
			sf::Vector2f(10, 10),
			{ TextLine({ CustomText{ .Text = "Game " + std::to_string(gameIndex), .Size = 18 }}) }, -1, false
	);

	while (window.isOpen())
	{
		sf::Event event{};

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				break;
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
			{
				gameIndex = (gameIndex + 1) % games.size();
				gameNumber = Text(
						sf::Vector2f(10, 10),
						{ TextLine({ CustomText{ .Text = "Game " + std::to_string(gameIndex), .Size = 18 }}) }, -1, false
				);
			}

			games[gameIndex].CheckInputs(event);
		}

		server.Update();

		sf::Time elapsed = clock.restart();

		for (auto& game : games)
		{
			game.Update(elapsed, sf::Vector2f(sf::Mouse::getPosition(window)));
		}

		window.clear();
		games[gameIndex].Draw(window);
		window.draw(gameNumber);
		window.display();
	}

	networkClientManager.Stop();
	networkServerManager.Running = false;

	return EXIT_SUCCESS;
}
