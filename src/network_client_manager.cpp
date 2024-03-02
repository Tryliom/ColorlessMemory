#include "network_client_manager.h"

#include "packet_manager.h"
#include "logger.h"

void NetworkClientManager::ReceivePackets(Client& client)
{
	while (running)
	{
		sf::Packet packet;
		PacketType packetType = PacketManager::ReceivePacket(*client.socket, packet);

		if (packetType == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet");
			std::exit(EXIT_FAILURE);
		}

		if (packetType == PacketType::Acknowledgement)
		{
			client.acknowledged = true;
			client.ackClock.restart();
			delete client.packetWaitingForAcknowledgement;
			continue;
		}

		if (onServerPacketReceived)
		{
			running = onServerPacketReceived(packet, packetType);
		}

		// Send acknowledgement packet
		client.SendPacket(PacketManager::CreatePacket(AcknowledgementPacket()));
	}
}

void NetworkClientManager::SendPackets(Client& client) const
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
}

void NetworkClientManager::SetOnMessageReceived(std::function<bool(sf::Packet&, PacketType)> onMessageReceived)
{
	this->onServerPacketReceived = std::move(onMessageReceived);
}

void NetworkClientManager::StartThreads(Client& client)
{
	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this, std::ref(client));
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this, std::ref(client));
	sendThread.detach();
}
