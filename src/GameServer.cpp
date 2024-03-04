#include "GameServer.h"

#include "Logger.h"

std::string ClientToString(sf::TcpSocket* client)
{
	return client->getRemoteAddress().toString() + ':' + std::to_string(client->getRemotePort());
}

namespace GameServer
{
	NetworkServerManager server;
	std::vector<LobbyData> lobbies;

	bool onReceivePacket(sf::TcpSocket* socket, Packet* packet);
	void joinLobby(sf::TcpSocket* socket);
	void removeFromLobby(sf::TcpSocket* socket);

	void Initialize()
	{
		// Bind the listener to a port
		if (!server.Bind(PORT))
		{
			LOG_ERROR("Could not bind listener to port");
			server.running = false;
			return;
		}

		LOG("Server is listening to port " << PORT);

		//TODO: Add a function to handle disconnects
		server.ListenToClientPackets(onReceivePacket);
		server.StartThreads();
	}

	void StartLoop()
	{
		while (server.running) {}
	}

	bool onReceivePacket(sf::TcpSocket* socket, Packet* packet)
	{
		if (packet->type == PacketType::JoinLobby)
		{
			LOG("Player " << ClientToString(socket) << " joined the lobby");
			joinLobby(socket);
		}
		else if (packet->type == PacketType::LeaveLobby)
		{
			LOG("Player " << ClientToString(socket) << " left the lobby");
			removeFromLobby(socket);
		}
		else if (packet->type == PacketType::StartGame)
		{
			// Find the lobby with the player
			for (auto& lobby : lobbies)
			{
				if (lobby.player1 == socket || lobby.player2 == socket)
				{
					// Send a message to the players that the game is starting
					PacketManager::SendPacket(*lobby.player1, new StartGamePacket(true));
					PacketManager::SendPacket(*lobby.player2, new StartGamePacket(false));
					return true;
				}
			}
		}

		return true;
	}

	void joinLobby(sf::TcpSocket* socket)
	{
		// Check if there is a lobby with only one player
		for (auto& lobby : lobbies)
		{
			if (lobby.player1 == nullptr)
			{
				lobby.player1 = socket;
				PacketManager::SendPacket(*socket, new JoinLobbyPacket(true, true));
				return;
			}
			else if (lobby.player2 == nullptr)
			{
				lobby.player2 = socket;
				// Send a message to the players that the lobby is full and player 1 can start the game
				PacketManager::SendPacket(*lobby.player1, new JoinLobbyPacket(true, false));
				PacketManager::SendPacket(*lobby.player2, new JoinLobbyPacket(false, false));
				return;
			}
		}

		// If there is no lobby with only one player, create a new lobby
		lobbies.push_back({ socket, nullptr });
		PacketManager::SendPacket(*socket, new JoinLobbyPacket(true, true));
	}

	void removeFromLobby(sf::TcpSocket* socket)
	{
		// Remove the player from the lobby
		for (auto& lobby : lobbies)
		{
			if (lobby.player1 == socket)
			{
				lobby.player1 = nullptr;

				if (lobby.player2 != nullptr)
				{
					joinLobby(lobby.player2);
					lobby.player2 = nullptr;
				}
			}
			else if (lobby.player2 == socket)
			{
				lobby.player2 = nullptr;

				if (lobby.player1 != nullptr)
				{
					joinLobby(lobby.player1);
					lobby.player1 = nullptr;
				}
			}
		}
	}
}