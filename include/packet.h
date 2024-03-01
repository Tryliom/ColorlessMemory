#pragma once

#include <SFML/Network.hpp>

enum class PacketType
{
	Connect,
	Disconnect,
	Message,
	Acknowledgement,
	Invalid
};

struct ConnectPacket
{
	std::string playerName;
};

struct DisconnectPacket
{
	std::string playerName;
};

struct MessagePacket
{
	std::string playerName;
	std::string message;
};

struct AcknowledgementPacket
{
	bool success = true;
};

sf::Packet& operator <<(sf::Packet& packet, const ConnectPacket& message);
sf::Packet& operator >>(sf::Packet& packet, ConnectPacket& message);

sf::Packet& operator <<(sf::Packet& packet, const DisconnectPacket& message);
sf::Packet& operator >>(sf::Packet& packet, DisconnectPacket& message);

sf::Packet& operator <<(sf::Packet& packet, const MessagePacket& message);
sf::Packet& operator >>(sf::Packet& packet, MessagePacket& message);

sf::Packet& operator <<(sf::Packet& packet, const AcknowledgementPacket& message);
sf::Packet& operator >>(sf::Packet& packet, AcknowledgementPacket& message);