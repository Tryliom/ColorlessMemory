#include "NetworkServerManager.h"
#include "Logger.h"

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

	server.ListenToClientPackets([&server](sf::TcpSocket* socket, Packet* packet)
	{
		if (packet->type == PacketType::Connect)
		{
			auto* connectPacket = dynamic_cast<ConnectPacket*>(packet);

			LOG("New connection received from " << connectPacket->playerName);
			server.SendMessageToAllClients(new MessagePacket("Server", connectPacket->playerName + " has joined the server"), socket);
		}

		if (packet->type == PacketType::Message)
		{
			auto* messageReceived = dynamic_cast<MessagePacket*>(packet);
			LOG("Message received from " << messageReceived->playerName << ": " << messageReceived->message);

			// Send a message to all clients
			server.SendMessageToAllClients(new MessagePacket(messageReceived->playerName, messageReceived->message), socket);
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
			server.SendMessageToAllClients(new MessagePacket { "Server", input });
		}
	}

	return EXIT_SUCCESS;
}