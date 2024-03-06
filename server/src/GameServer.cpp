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

	void OnReceivePacket(sf::TcpSocket* socket, Packet* packet);
	void OnDisconnect(sf::TcpSocket* socket);
	void JoinLobby(sf::TcpSocket* socket, const std::string& name, std::size_t iconIndex);
	void RemoveFromLobby(sf::TcpSocket* socket);

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
		if (packet->type == PacketType::JoinLobby)
		{
			const auto* joinLobbyPacket = dynamic_cast<JoinLobbyPacket*>(packet);
			LOG("Player " << ClientToString(socket) << " aka " << joinLobbyPacket->Name << " joined the lobby");
			JoinLobby(socket, joinLobbyPacket->Name, joinLobbyPacket->IconIndex);
		}
		else if (packet->type == PacketType::ChangeDeck)
		{
			// Find the lobby with the player
			for (auto& lobby : lobbies)
			{
				if (lobby.player1 == socket || lobby.player2 == socket)
				{
					lobby.deckType = dynamic_cast<ChangeDeckPacket*>(packet)->DeckType;
					break;
				}
			}
		}
		else if (packet->type == PacketType::LeaveLobby)
		{
			LOG("Player " << ClientToString(socket) << " left the lobby");
			RemoveFromLobby(socket);
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
					break;
				}
			}
		}
	}

	void OnDisconnect(sf::TcpSocket* socket)
	{
		LOG("Player " << ClientToString(socket) << " disconnected");
		RemoveFromLobby(socket);
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
}