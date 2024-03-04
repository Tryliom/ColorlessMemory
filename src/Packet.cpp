#include "Packet.h"

Packet* Packet::FromType(PacketType type)
{
	switch (type)
	{
	case PacketType::Connect: return new ConnectPacket();
	case PacketType::Message: return new MessagePacket();
	default: return new InvalidPacket();
	}
}

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType)
{
	packet << static_cast<sf::Uint8>(packetType.type);

	switch (packetType.type)
	{
	case PacketType::Connect:
	{
		const auto& connectPacket = dynamic_cast<const ConnectPacket&>(packetType);
		packet << connectPacket;
		break;
	}
	case PacketType::Message:
	{
		const auto& messagePacket = dynamic_cast<const MessagePacket&>(packetType);
		packet << messagePacket;
		break;
	}
	default:
		break;
	}

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Packet& packetType)
{
	switch (packetType.type)
	{
	case PacketType::Connect:
	{
		auto* connectPacket = dynamic_cast<ConnectPacket*>(&packetType);
		packet >> *connectPacket;
		break;
	}
	case PacketType::Message:
	{
		auto* messagePacket = dynamic_cast<MessagePacket*>(&packetType);
		packet >> *messagePacket;
		break;
	}
	default:
		break;
	}

	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectPacket& connectPacket)
{
	return packet << connectPacket.playerName;
}

sf::Packet& operator >>(sf::Packet& packet, ConnectPacket& connectPacket)
{
	packet >> connectPacket.playerName;
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const MessagePacket& message)
{
	return packet << message.playerName << message.message;
}

sf::Packet& operator >>(sf::Packet& packet, MessagePacket& message)
{
	packet >> message.playerName >> message.message;
	return packet;
}