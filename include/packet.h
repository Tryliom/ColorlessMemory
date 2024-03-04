#pragma once

#include <SFML/Network.hpp>
#include <utility>

enum class PacketType
{
	Connect,
	Message,
	Acknowledgement,
	Invalid
};

// Packet attributes always need to be initialized to default values

struct Packet
{
	Packet() = default;
	explicit Packet(PacketType type) : type(type) {}
	virtual ~Packet() = default;

	PacketType type = PacketType::Invalid;
};

struct ConnectPacket final : Packet
{
	ConnectPacket() : Packet(PacketType::Connect) {}
	explicit ConnectPacket(std::string_view playerName) : Packet(PacketType::Connect), playerName(playerName) {}

	std::string playerName;
};

struct MessagePacket final : Packet
{
	MessagePacket() : Packet(PacketType::Message) {}
	MessagePacket(std::string_view playerName, std::string_view message) :
		Packet(PacketType::Message), playerName(playerName), message(message) {}

	std::string playerName;
	std::string message;
};

struct AcknowledgementPacket final : Packet
{
	AcknowledgementPacket() : Packet(PacketType::Acknowledgement) {}

	bool success = true;
};

struct InvalidPacket final : Packet
{
	InvalidPacket() : Packet(PacketType::Invalid) {}
};

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType);
sf::Packet& operator >>(sf::Packet& packet, Packet& packetType);

sf::Packet& operator <<(sf::Packet& packet, const ConnectPacket& connectPacket);
sf::Packet& operator >>(sf::Packet& packet, ConnectPacket& connectPacket);

sf::Packet& operator <<(sf::Packet& packet, const MessagePacket& message);
sf::Packet& operator >>(sf::Packet& packet, MessagePacket& message);

sf::Packet& operator <<(sf::Packet& packet, const AcknowledgementPacket& message);
sf::Packet& operator >>(sf::Packet& packet, AcknowledgementPacket& message);