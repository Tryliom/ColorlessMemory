#include "packet.h"

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType)
{
	packet << static_cast<sf::Uint8>(packetType.type);

	switch (packetType.type)
	{
	case PacketType::Connect:
	{
		const auto& connectPacket = static_cast<const ConnectPacket&>(packetType);
		packet << connectPacket;
		break;
	}
	case PacketType::Message:
	{
		const auto& messagePacket = static_cast<const MessagePacket&>(packetType);
		packet << messagePacket;
		break;
	}
	case PacketType::Acknowledgement:
	{
		const auto& ackPacket = static_cast<const AcknowledgementPacket&>(packetType);
		packet << ackPacket;
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
	case PacketType::Acknowledgement:
	{
		auto* ackPacket = dynamic_cast<AcknowledgementPacket*>(&packetType);
		packet >> *ackPacket;
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

sf::Packet& operator <<(sf::Packet& packet, const AcknowledgementPacket& message)
{
	return packet << message.success;
}

sf::Packet& operator >>(sf::Packet& packet, AcknowledgementPacket& message)
{
	packet >> message.success;
	return packet;
}