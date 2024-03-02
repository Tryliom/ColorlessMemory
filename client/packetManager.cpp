#include "packetManager.h"

#include "logger.h"

namespace PacketManager
{
	bool SendPacket(sf::TcpSocket& socket, const ConnectPacket& packet)
	{
		auto* p = CreatePacket(packet);
		auto status = socket.send(*p);
		delete p;

		return status == sf::Socket::Done;
	}

	bool SendPacket(sf::TcpSocket& socket, const DisconnectPacket& packet)
	{
		auto* p = CreatePacket(packet);
		auto status = socket.send(*p);
		delete p;

		return status == sf::Socket::Done;
	}

	bool SendPacket(sf::TcpSocket& socket, const MessagePacket& packet)
	{
		auto* p = CreatePacket(packet);
		auto status = socket.send(*p);
		delete p;

		return status == sf::Socket::Done;
	}

	bool SendPacket(sf::TcpSocket& socket, const AcknowledgementPacket& packet)
	{
		auto* p = CreatePacket(packet);
		auto status = socket.send(*p);
		delete p;

		return status == sf::Socket::Done;
	}

	PacketType GetPacketType(sf::Packet& packet)
	{
		sf::Uint8 packetType;
		packet >> packetType;
		return static_cast<PacketType>(packetType);
	}

	PacketType ReceivePacket(sf::TcpSocket& socket, sf::Packet& packet)
	{
		if (socket.receive(packet) != sf::Socket::Done)
		{
			LOG_ERROR("Could not receive packet");
			return PacketType::Invalid;
		}

		return GetPacketType(packet);
	}

	ConnectPacket GetConnectPacket(sf::Packet& packet)
	{
		ConnectPacket p;
		packet >> p;
		return p;
	}

	DisconnectPacket GetDisconnectPacket(sf::Packet& packet)
	{
		DisconnectPacket p;
		packet >> p;
		return p;
	}

	MessagePacket GetMessagePacket(sf::Packet& packet)
	{
		MessagePacket p;
		packet >> p;
		return p;
	}

	sf::Packet* CreatePacket(const ConnectPacket& packet)
	{
		auto* p = new sf::Packet();
		*p << packet;
		return p;
	}

	sf::Packet* CreatePacket(const DisconnectPacket& packet)
	{
		auto* p = new sf::Packet();
		*p << packet;
		return p;
	}

	sf::Packet* CreatePacket(const MessagePacket& packet)
	{
		auto* p = new sf::Packet();
		*p << packet;
		return p;
	}

	sf::Packet* CreatePacket(const AcknowledgementPacket& packet)
	{
		auto* p = new sf::Packet();
		*p << packet;
		return p;
	}
}