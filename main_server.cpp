#include "network_server_manager.h"
#include "logger.h"

int main()
{
	NetworkServerManager server;

	// Bind the listener to a port
	if (!server.Bind(PORT))
	{
		LOG_ERROR("Could not bind listener to port");
		return EXIT_FAILURE;
	}

	LOG("Server is listening to port " << PORT);

	server.ListenToClientPackets([&server](sf::TcpSocket* socket, const PacketType packetType, sf::Packet packet)
	{
		if (packetType == PacketType::Connect)
		{
			ConnectPacket connectPacket = PacketManager::GetConnectPacket(packet);

			LOG("New connection received from " << connectPacket.playerName);
			server.SendMessageToAllClients(MessagePacket{ "Server", connectPacket.playerName + " has joined the server" }, socket);
		}

		if (packetType == PacketType::Message)
		{
			MessagePacket messageReceived = PacketManager::GetMessagePacket(packet);
			LOG("Message received from " << messageReceived.playerName << ": " << messageReceived.message);

			// Send a message to all clients
			server.SendMessageToAllClients(messageReceived, socket);
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
			server.SendMessageToAllClients(MessagePacket { "Server", input });
		}
	}

	return EXIT_SUCCESS;
}