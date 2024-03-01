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
			if (client.socket->receive(answer) != sf::Socket::Done)
			{
				LOG_ERROR("Could not receive answer from server");
				std::exit(EXIT_FAILURE);
			}

			LOG("Received packet from server");

			PacketType packetType = PacketManager::ReceivePacket(*client.socket, answer);

			if (packetType == PacketType::Message)
			{
				MessagePacket messageReceived = PacketManager::GetMessagePacket(answer);
				LOG(messageReceived.playerName + ": " + messageReceived.message);
			}
			else if (packetType == PacketType::Acknowledgement)
			{
				LOG("Acknowledgement received");
				client.acknowledged = true;
				client.ackClock.restart();
				delete client.packetWaitingForAcknowledgement;
				continue;
			}
			else
			{
				LOG_ERROR("Received invalid packet type");
			}

			// Send acknowledgement packet
			client.acknowledged = false;
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
					if (client.socket->send(*client.packetWaitingForAcknowledgement) != sf::Socket::Done)
					{
						LOG_ERROR("Could not send packet to server");
						std::exit(EXIT_FAILURE);
					}

					client.ackClock.restart();
				}
			}
			else if (!client.packetsToBeSent.empty())
			{
				client.packetWaitingForAcknowledgement = client.packetsToBeSent.front();
				client.packetsToBeSent.pop();

				if (client.socket->send(*client.packetWaitingForAcknowledgement) != sf::Socket::Done)
				{
					LOG_ERROR("Could not send packet to server");
					std::exit(EXIT_FAILURE);
				}

				if (PacketManager::GetPacketType(*client.packetWaitingForAcknowledgement) != PacketType::Acknowledgement)
				{
					client.acknowledged = false;
					client.ackClock.restart();
				}
				else
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
			if (!PacketManager::SendPacket(*client.socket, disconnectPacket))
			{
				LOG_ERROR("Could not send disconnect packet");
				return EXIT_FAILURE;
			}
			continue;
		}

		client.SendPacket(PacketManager::CreatePacket(messageSent));
	}

	return EXIT_SUCCESS;
}