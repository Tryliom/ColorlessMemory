#pragma once

#include <SFML/Network.hpp>
#include <utility>

enum class PacketType
{
	JoinLobby,
	LeaveLobby,
	StartGame,
	Invalid
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

	//TODO: Remove this and pass informations with UpdateGamePacket which contains the turn, score, etc
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