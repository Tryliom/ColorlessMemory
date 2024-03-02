#include "packetManager.h"
#include "client.h"
#include "logger.h"

#include <thread>
#include <queue>

int main()
{
	bool running = true;
	Client client;
	sf::Socket::Status status = client.socket->connect(HOST_NAME, PORT);

	if (status != sf::Socket::Done)
	{
		LOG_ERROR("Could not connect to server");
		return EXIT_FAILURE;
	}

	std::thread receiveThread([&client, &running]()
	{
		while (running)
		{
			sf::Packet answer;
			PacketType packetType = PacketManager::ReceivePacket(*client.socket, answer);

			if (packetType == PacketType::Invalid)
			{
				LOG_ERROR("Could not receive answer from server");
				std::exit(EXIT_FAILURE);
			}

			if (packetType == PacketType::Message)
			{
				MessagePacket messageReceived = PacketManager::GetMessagePacket(answer);
				LOG(messageReceived.playerName + ": " + messageReceived.message);
			}
			else if (packetType == PacketType::Acknowledgement)
			{
				client.acknowledged = true;
				client.ackClock.restart();
				delete client.packetWaitingForAcknowledgement;
				continue;
			}

			// Send acknowledgement packet
			client.SendPacket(PacketManager::CreatePacket(AcknowledgementPacket()));
		}
	});
	receiveThread.detach();

	std::thread sendThread([&client, &running]()
	{
		while (running)
		{
			if (!client.acknowledged)
			{
				if (client.ackClock.getElapsedTime().asMilliseconds() > Client::ACK_TIMEOUT)
				{
					// Resend packet
					client.socket->send(*client.packetWaitingForAcknowledgement);
					client.ackClock.restart();
				}
			}
			else if (!client.packetsToBeSent.empty())
			{
				client.packetWaitingForAcknowledgement = client.packetsToBeSent.front();
				client.packetsToBeSent.pop();

				if (PacketManager::GetPacketType(*client.packetWaitingForAcknowledgement) != PacketType::Acknowledgement)
				{
					client.acknowledged = false;
					client.ackClock.restart();
				}

				client.socket->send(*client.packetWaitingForAcknowledgement);

				if (PacketManager::GetPacketType(*client.packetWaitingForAcknowledgement) == PacketType::Acknowledgement)
				{
					client.acknowledged = true;
					client.ackClock.restart();
					delete client.packetWaitingForAcknowledgement;
				}
			}
		}
	});
	sendThread.detach();

	ConnectPacket connectPacket;
	std::cout << "Enter your name: ";
	std::cin >> connectPacket.playerName;

	client.SendPacket(PacketManager::CreatePacket(connectPacket));

	LOG("Chat + Enter to send | stop to exit");

	while(running)
	{
		MessagePacket messageSent;
		messageSent.playerName = connectPacket.playerName;
		std::cin >> messageSent.message;

		if (messageSent.message == "stop")
		{
			running = false;
			DisconnectPacket disconnectPacket;
			disconnectPacket.playerName = connectPacket.playerName;
			PacketManager::SendPacket(*client.socket, disconnectPacket);
			continue;
		}

		client.SendPacket(PacketManager::CreatePacket(messageSent));
	}

	return EXIT_SUCCESS;
}