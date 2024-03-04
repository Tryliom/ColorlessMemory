#include "network_client_manager.h"

#include "packet_manager.h"
#include "logger.h"

void NetworkClientManager::ReceivePackets(Client& client)
{
	while (running)
	{
		Packet* packet = PacketManager::ReceivePacket(*client.socket);

		if (packet->type == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet");
			std::exit(EXIT_FAILURE);
		}

		if (packet->type == PacketType::Acknowledgement)
		{
			client.acknowledged = true;
			client.ackClock.restart();
			delete client.packetWaitingForAcknowledgement;
			continue;
		}

		if (onServerPacketReceived)
		{
			running = onServerPacketReceived(*packet);
		}

		// Send acknowledgement packet
		client.SendPacket(new AcknowledgementPacket());

		delete packet;
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
				client.socket->send(*PacketManager::CreatePacket(client.packetWaitingForAcknowledgement));
				client.ackClock.restart();
			}
		}
		else if (!client.packetsToBeSent.empty())
		{
			client.packetWaitingForAcknowledgement = client.packetsToBeSent.front();
			client.packetsToBeSent.pop();

			if (client.packetWaitingForAcknowledgement->type != PacketType::Acknowledgement)
			{
				client.acknowledged = false;
				client.ackClock.restart();
			}

			client.socket->send(*PacketManager::CreatePacket(client.packetWaitingForAcknowledgement));

			if (client.packetWaitingForAcknowledgement->type == PacketType::Acknowledgement)
			{
				client.acknowledged = true;
				client.ackClock.restart();
				delete client.packetWaitingForAcknowledgement;
			}
		}
	}
}

void NetworkClientManager::SetOnMessageReceived(const std::function<bool(const Packet&)>& onMessageReceived)
{
	this->onServerPacketReceived = onMessageReceived;
}

void NetworkClientManager::StartThreads(Client& client)
{
	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this, std::ref(client));
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this, std::ref(client));
	sendThread.detach();
}
