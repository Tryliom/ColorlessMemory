#include "server.h"
#include "logger.h"

int main()
{
	Server server;

	// Bind the listener to a port
	if (!server.Bind(PORT))
	{
		LOG_ERROR("Could not bind listener to port");
		return EXIT_FAILURE;
	}

	LOG("Server is listening to port " << PORT);

	server.ListenToClientPackets([&server](sf::TcpSocket* socket, const PacketType packetType, sf::Packet packet)
	{
		switch (packetType)
		{
		case PacketType::Connect:
		{
			ConnectPacket connectPacket = PacketManager::GetConnectPacket(packet);
			LOG("New connection received from " << connectPacket.playerName);

			// Send a message to all clients
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = connectPacket.playerName + " has joined the server";

			server.SendMessageToAllClients(messageSent, socket);
			break;
		}
		case PacketType::Disconnect:
		{
			DisconnectPacket disconnectPacket = PacketManager::GetDisconnectPacket(packet);
			LOG("Connection lost from " << disconnectPacket.playerName);

			// Send a message to all clients
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = disconnectPacket.playerName + " has left the server";

			server.SendMessageToAllClients(messageSent, socket);
			return false;
		}
		case PacketType::Message:
		{
			MessagePacket messageReceived = PacketManager::GetMessagePacket(packet);
			LOG("Message received from " << messageReceived.playerName << ": " << messageReceived.message);

			// Send a message to all clients
			server.SendMessageToAllClients(messageReceived, socket);
			break;
		}
		default:
			LOG_ERROR("Unknown packet type received");
			break;
		}

		return true;
	});
	server.StartThreads();

	while (server.running)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			server.running = false;
			break;
		}
		else
		{
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = input;

			server.SendMessageToAllClients(messageSent);
		}
	}

	return EXIT_SUCCESS;
}