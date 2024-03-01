#include "packet.h"

sf::Packet& operator <<(sf::Packet& packet, const ConnectPacket& message)
{
	return packet << static_cast<sf::Uint8>(PacketType::Connect) << message.playerName;
}

sf::Packet& operator >>(sf::Packet& packet, ConnectPacket& message)
{
	packet >> message.playerName;
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const DisconnectPacket& message)
{
	return packet << static_cast<sf::Uint8>(PacketType::Disconnect) << message.playerName;
}

sf::Packet& operator >>(sf::Packet& packet, DisconnectPacket& message)
{
	packet >> message.playerName;
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const MessagePacket& message)
{
	return packet << static_cast<sf::Uint8>(PacketType::Message) << message.playerName << message.message;
}

sf::Packet& operator >>(sf::Packet& packet, MessagePacket& message)
{
	packet >> message.playerName >> message.message;
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const AcknowledgementPacket& message)
{
	return packet << static_cast<sf::Uint8>(PacketType::Acknowledgement) << message.success;
}

sf::Packet& operator >>(sf::Packet& packet, AcknowledgementPacket& message)
{
	packet >> message.success;
	return packet;
}