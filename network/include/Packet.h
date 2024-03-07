#pragma once

#include "DeckType.h"

#include <SFML/Network.hpp>

#include <utility>

enum class PacketType
{
	LobbyInformation,
	ChangeDeck,
	JoinLobby,
	LeaveLobby,
	StartGame,
	Turn,
	CardInformation,
	Invalid // Always last
};

std::string PacketTypeToString(PacketType type);

// Packet attributes always need to be initialized to default values

struct Packet
{
	Packet() = default;
	explicit Packet(PacketType type) : type(type) {}
	virtual ~Packet() = default;

	PacketType type = PacketType::Invalid;

	static Packet* FromType(PacketType type);
};

struct LobbyInformationPacket final : Packet
{
	LobbyInformationPacket() : Packet(PacketType::LobbyInformation) {}
	LobbyInformationPacket(bool isHost, bool waitingForOpponent, std::string player1Name, std::string player2Name, std::size_t player1Icon,
		std::size_t player2Icon, DeckType deckType)
		: Packet(PacketType::LobbyInformation), IsHost(isHost), WaitingForOpponent(waitingForOpponent), Player1Name(std::move(player1Name)),
		Player2Name(std::move(player2Name)), Player1Icon(player1Icon), Player2Icon(player2Icon), DeckType(deckType) {}

	bool IsHost{};
	bool WaitingForOpponent{};
	std::string Player1Name{};
	std::string Player2Name{};
	std::size_t Player1Icon{};
	std::size_t Player2Icon{};
	DeckType DeckType { DeckType::Deck3x2 };
};

struct ChangeDeckPacket final : Packet
{
	ChangeDeckPacket() : Packet(PacketType::ChangeDeck) {}
	explicit ChangeDeckPacket(DeckType deckType) : Packet(PacketType::ChangeDeck), DeckType(deckType) {}

	DeckType DeckType{};
};

struct JoinLobbyPacket final : Packet
{
	JoinLobbyPacket() : Packet(PacketType::JoinLobby) {}
	explicit JoinLobbyPacket(std::string name, std::size_t iconIndex) : Packet(PacketType::JoinLobby), Name(std::move(name)), IconIndex(iconIndex) {}

	std::string Name;
	std::size_t IconIndex{};
};

struct LeaveLobbyPacket final : Packet
{
	LeaveLobbyPacket() : Packet(PacketType::LeaveLobby) {}
};

struct StartGamePacket final : Packet
{
	StartGamePacket() : Packet(PacketType::StartGame) {}
	explicit StartGamePacket(DeckType deckType, bool yourTurn) : Packet(PacketType::StartGame), DeckType(deckType), YourTurn(yourTurn) {}

	DeckType DeckType{};
	bool YourTurn{};
};

struct TurnPacket final : Packet
{
	TurnPacket() : Packet(PacketType::Turn) {}
	explicit TurnPacket(bool yourTurn) : Packet(PacketType::Turn), YourTurn(yourTurn) {}

	bool YourTurn{};
};

struct CardInformationPacket final : Packet
{
	CardInformationPacket() : Packet(PacketType::CardInformation) {}
	explicit CardInformationPacket(std::size_t cardIndex, int iconIndex) : Packet(PacketType::CardInformation), CardIndex(cardIndex), IconIndex(iconIndex) {}

	int CardIndex{};
	std::size_t IconIndex{};
};

struct InvalidPacket final : Packet
{
	InvalidPacket() : Packet(PacketType::Invalid) {}
};

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType);
sf::Packet& operator >>(sf::Packet& packet, Packet& packetType);