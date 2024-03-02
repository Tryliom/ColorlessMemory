#include "packet_manager.h"
#include "network_client_manager.h"
#include "logger.h"

int main()
{
	bool running = true;
	Client client;
	NetworkClientManager networkClientManager;
	sf::Socket::Status status = client.socket->connect(HOST_NAME, PORT);

	if (status != sf::Socket::Done)
	{
		LOG_ERROR("Could not connect to server");
		return EXIT_FAILURE;
	}

	networkClientManager.SetOnMessageReceived([](sf::Packet& packet, PacketType packetType)
	{
		if (packetType == PacketType::Message)
		{
			MessagePacket messageReceived = PacketManager::GetMessagePacket(packet);
			LOG(messageReceived.playerName + ": " + messageReceived.message);
		}

		return true;
	});

	networkClientManager.StartThreads(client);

	ConnectPacket connectPacket;
	std::cout << "Enter your name: ";
	std::getline(std::cin, connectPacket.playerName);

	client.SendPacket(PacketManager::CreatePacket(connectPacket));

	LOG("Chat + Enter to send | stop to exit");

	while(running)
	{
		MessagePacket messageSent;
		messageSent.playerName = connectPacket.playerName;
		std::getline(std::cin, messageSent.message);

		if (messageSent.message == "stop")
		{
			running = false;
			continue;
		}

		client.SendPacket(PacketManager::CreatePacket(messageSent));
	}

	return EXIT_SUCCESS;
}