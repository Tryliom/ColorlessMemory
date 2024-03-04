#include "packet_manager.h"

#include "logger.h"

namespace PacketManager
{
	bool SendPacket(sf::TcpSocket& socket, Packet* packet)
	{
		auto* p = CreatePacket(packet);
		auto status = socket.send(*p);
		delete p;

		return status == sf::Socket::Done;
	}

	Packet* ReceivePacket(sf::TcpSocket& socket)
	{
		sf::Packet packet;
		if (socket.receive(packet) != sf::Socket::Done)
		{
			LOG_ERROR("Could not receive packet");
			return new InvalidPacket();
		}

		sf::Uint8 packetTypeUint;
		packet >> packetTypeUint;
		auto packetType = static_cast<PacketType>(packetTypeUint);

		Packet* ourPacket;
		switch (packetType)
		{
		case PacketType::Connect:
			ourPacket = new ConnectPacket();
			break;
		case PacketType::Message:
			ourPacket = new MessagePacket();
			break;
		case PacketType::Acknowledgement:
			ourPacket = new AcknowledgementPacket();
			break;
		default:
			ourPacket = new InvalidPacket();
			break;
		}

		packet >> *ourPacket;

		return ourPacket;
	}

	sf::Packet* CreatePacket(Packet* packet)
	{
		auto* p = new sf::Packet();
		*p << *packet;
		return p;
	}
}