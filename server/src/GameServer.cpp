#include "GameServer.h"

#include "Logger.h"
#include "Random.h"

std::string ClientToString(sf::TcpSocket* client)
{
	return client->getRemoteAddress().toString() + ':' + std::to_string(client->getRemotePort());
}

Game::Game(const Lobby& lobbyData)
{
	player1 = lobbyData.player1;
	player2 = lobbyData.player2;

	// Create deck
	std::size_t cardCount = 0;
	switch (lobbyData.deckType)
	{
	case DeckType::Deck3x2:
		cardCount = 6;
		break;
	case DeckType::Deck7x2:
		cardCount = 14;
		break;
	case DeckType::Deck6x5:
		cardCount = 30;
		break;
	case DeckType::Deck7x6:
		cardCount = 42;
		break;
	case DeckType::Deck10x5:
		cardCount = 50;
		break;
	}

	const auto& maxCard = cardCount / 2;
	for (std::size_t i = 0; i < maxCard; i++)
	{
		cards.push_back(i);
		cards.push_back(i);
	}

	Random::Shuffle(cards);

	turn = Random::Range(0, 1);
	selectedCards = { -1, -1 };
}

namespace GameServer
{
	NetworkServerManager server;
	std::vector<Lobby> lobbies;
	std::vector<Game> games;

	void OnReceivePacket(sf::TcpSocket* socket, Packet* packet);
	void OnDisconnect(sf::TcpSocket* socket);
	void JoinLobby(sf::TcpSocket* socket, const std::string& name, std::size_t iconIndex);
	void RemoveFromLobby(sf::TcpSocket* socket);
	void RemoveFromGame(sf::TcpSocket* socket);

	void Initialize()
	{
		// Bind the listener to a port
		if (!server.Bind(PORT))
		{
			LOG_ERROR("Could not bind listener to port");
			server.Running = false;
			return;
		}

		LOG("Server is listening to port " << PORT);

		server.OnPacketReceived(OnReceivePacket);
		server.OnDisconnect(OnDisconnect);
		server.StartThreads();
	}

	void StartLoop()
	{
		while (server.Running) {}
	}

	void OnReceivePacket(sf::TcpSocket* socket, Packet* packet)
	{
		if (packet->Type == PacketType::JoinLobby)
		{
			const auto* joinLobbyPacket = dynamic_cast<JoinLobbyPacket*>(packet);
			LOG("Player " << ClientToString(socket) << " aka " << joinLobbyPacket->Name << " joined the lobby");
			JoinLobby(socket, joinLobbyPacket->Name, joinLobbyPacket->IconIndex);
		}
		else if (packet->Type == PacketType::ChangeDeck)
		{
			// Find the lobby with the player
			for (auto& lobby : lobbies)
			{
				if (lobby.player1 == socket || lobby.player2 == socket)
				{
					lobby.deckType = dynamic_cast<ChangeDeckPacket*>(packet)->ChosenDeckType;
					break;
				}
			}
		}
		else if (packet->Type == PacketType::LeaveLobby)
		{
			LOG("Player " << ClientToString(socket) << " left the lobby");
			RemoveFromLobby(socket);
		}
		else if (packet->Type == PacketType::StartGame)
		{
			// Find the lobby with the player
			for (auto& lobby : lobbies)
			{
				if (lobby.player1 == socket || lobby.player2 == socket)
				{
					// Create a new game
					games.emplace_back(lobby);
					const auto& game = games.back();

					// Send a message to the players that the game is starting
					PacketManager::SendPacket(*lobby.player1, new StartGamePacket(lobby.deckType, game.turn == 0));
					PacketManager::SendPacket(*lobby.player2, new StartGamePacket(lobby.deckType, game.turn == 1));

					// Remove the lobby
					lobby.Reset();
					break;
				}
			}
		}
		else if (packet->Type == PacketType::CardInformation)
		{
			const auto* cardInformationPacket = dynamic_cast<CardInformationPacket*>(packet);
			auto gameToDelete = -1;

			// Find the game with the player
			for (auto i = 0; i < games.size(); i++)
			{
				auto& game = games[i];

				if (game.player1 != socket && game.player2 != socket) continue;

				auto playerTurn = game.turn;
				auto requestPlayer = game.player1 == socket ? 0 : 1;

				if (playerTurn != requestPlayer) return;
				if (game.HasSelectedTwoCards()) return;
				if (game.selectedCards[0] == cardInformationPacket->CardIndexInDeck) return;

				game.SelectCard(cardInformationPacket->CardIndexInDeck);

				const int iconIndex = static_cast<int>(game.cards[cardInformationPacket->CardIndexInDeck]);

				PacketManager::SendPacket(*game.player1, new CardInformationPacket(cardInformationPacket->CardIndexInDeck, iconIndex));
				PacketManager::SendPacket(*game.player2, new CardInformationPacket(cardInformationPacket->CardIndexInDeck, iconIndex));

				if (game.HasSelectedTwoCards())
				{
					if (game.cards[game.selectedCards[0]] != game.cards[game.selectedCards[1]])
					{
						game.turn = 1 - game.turn;
					}
					else
					{
						game.totalScore++;
					}

					game.UnselectCards();

					if (game.IsGameOver())
					{
						// Remove game
						gameToDelete = i;
					}
					else
					{
						// Send a message to the players whose turn it is
						PacketManager::SendPacket(*game.player1, new TurnPacket(game.turn == 0));
						PacketManager::SendPacket(*game.player2, new TurnPacket(game.turn == 1));
					}
				}

				break;
			}

			if (gameToDelete != -1)
			{
				games.erase(games.begin() + gameToDelete);
			}
		}
		else if (packet->Type == PacketType::LeaveGame)
		{
			LOG("Player " << ClientToString(socket) << " left the game");
			RemoveFromGame(socket);
		}
	}

	void OnDisconnect(sf::TcpSocket* socket)
	{
		LOG("Player " << ClientToString(socket) << " disconnected");
		RemoveFromLobby(socket);
		RemoveFromGame(socket);
	}

	void JoinLobby(sf::TcpSocket* socket, const std::string& name, std::size_t iconIndex)
	{
		// Check if there is a lobby with only one player
		for (auto& lobby : lobbies)
		{
			if (lobby.player1 == nullptr)
			{
				lobby.player1 = socket;
				lobby.player1Name = name;
				lobby.player1Icon = iconIndex;
				PacketManager::SendPacket(*socket, lobby.ToPacket(true));
				return;
			}
			else if (lobby.player2 == nullptr)
			{
				lobby.player2 = socket;
				lobby.player2Name = name;
				lobby.player2Icon = iconIndex;
				// Send a message to the players that the lobby is full and player 1 can start the game
				PacketManager::SendPacket(*lobby.player1, lobby.ToPacket(true));
				PacketManager::SendPacket(*lobby.player2, lobby.ToPacket(false));
				return;
			}
		}

		// If there is no lobby with only one player, create a new lobby
		lobbies.push_back({ socket, nullptr, name, "", iconIndex, 0 });
		PacketManager::SendPacket(*socket, lobbies.back().ToPacket(true));
	}

	void RemoveFromLobby(sf::TcpSocket* socket)
	{
		// Remove the player from the lobby
		for (auto& lobby : lobbies)
		{
			if (lobby.player1 == socket)
			{
				lobby.player1 = nullptr;

				if (lobby.player2 != nullptr)
				{
					auto* player = lobby.player2;
					auto name = lobby.player2Name;
					auto iconIndex = lobby.player2Icon;
					lobby.Reset();
					JoinLobby(player, name, iconIndex);
				}
			}
			else if (lobby.player2 == socket)
			{
				lobby.player2 = nullptr;

				if (lobby.player1 != nullptr)
				{
					auto* player = lobby.player1;
					auto name = lobby.player1Name;
					auto iconIndex = lobby.player1Icon;
					lobby.Reset();
					JoinLobby(player, name, iconIndex);
				}
			}
		}
	}

	void RemoveFromGame(sf::TcpSocket* socket)
	{
		// Remove the player from the game
		for (auto i = 0; i < games.size(); i++)
		{
			auto& game = games[i];

			if (game.player1 == socket || game.player2 == socket)
			{
				// Send a message to the other player that the opponent left the game
				if (game.player1 == socket)
				{
					PacketManager::SendPacket(*game.player2, new LeaveGamePacket());
				}
				else
				{
					PacketManager::SendPacket(*game.player1, new LeaveGamePacket());
				}

				games.erase(games.begin() + i);
				break;
			}
		}
	}
}