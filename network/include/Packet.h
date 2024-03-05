#pragma once

#include <SFML/Network.hpp>

#include <utility>

enum class PacketType
{
	JoinLobby,
	LeaveLobby,
	StartGame,
	Invalid // Always last
};

// Packet attributes always need to be initialized to default values

struct Packet
{
	Packet() = default;
	explicit Packet(PacketType type) : type(type) {}
	virtual ~Packet() = default;

	PacketType type = PacketType::Invalid;

	static Packet* FromType(PacketType type);
};

struct JoinLobbyPacket final : Packet
{
	JoinLobbyPacket() : Packet(PacketType::JoinLobby) {}
	JoinLobbyPacket(bool isHost, bool waitingForOpponent) : Packet(PacketType::JoinLobby), IsHost(isHost), WaitingForOpponent(waitingForOpponent) {}

	bool IsHost{};
	bool WaitingForOpponent{};
};

struct LeaveLobbyPacket final : Packet
{
	LeaveLobbyPacket() : Packet(PacketType::LeaveLobby) {}
};

struct StartGamePacket final : Packet
{
	StartGamePacket() : Packet(PacketType::StartGame) {}
	explicit StartGamePacket(bool yourTurn) : Packet(PacketType::StartGame), YourTurn(yourTurn) {}

	//TODO: Use TurnPacket to tell the client that it's their turn
	//TODO: Use CardPacket to tell the client which icon is on the card selected and where it is
	//TODO: When last cardPacket sent, the server remove the lobby and game ends, everything is handle by the client
	bool YourTurn{};
};

struct InvalidPacket final : Packet
{
	InvalidPacket() : Packet(PacketType::Invalid) {}
};

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType);
sf::Packet& operator >>(sf::Packet& packet, Packet& packetType);

sf::Packet& operator <<(sf::Packet& packet, const JoinLobbyPacket& joinLobbyPacket);
sf::Packet& operator >>(sf::Packet& packet, JoinLobbyPacket& joinLobbyPacket);

sf::Packet& operator <<(sf::Packet& packet, const LeaveLobbyPacket& leaveLobbyPacket);
sf::Packet& operator >>(sf::Packet& packet, LeaveLobbyPacket& leaveLobbyPacket);

sf::Packet& operator <<(sf::Packet& packet, const StartGamePacket& startGamePacket);
sf::Packet& operator >>(sf::Packet& packet, StartGamePacket& startGamePacket);