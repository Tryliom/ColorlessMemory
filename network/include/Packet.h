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

sf::Packet& operator <<(sf::Packet& packet, const LobbyInformationPacket& joinLobbyPacket);
sf::Packet& operator >>(sf::Packet& packet, LobbyInformationPacket& joinLobbyPacket);

sf::Packet& operator <<(sf::Packet& packet, const ChangeDeckPacket& changeDeckPacket);
sf::Packet& operator >>(sf::Packet& packet, ChangeDeckPacket& changeDeckPacket);

sf::Packet& operator <<(sf::Packet& packet, const JoinLobbyPacket& joinLobbyPacket);
sf::Packet& operator >>(sf::Packet& packet, JoinLobbyPacket& joinLobbyPacket);

sf::Packet& operator <<(sf::Packet& packet, const LeaveLobbyPacket& leaveLobbyPacket);
sf::Packet& operator >>(sf::Packet& packet, LeaveLobbyPacket& leaveLobbyPacket);

sf::Packet& operator <<(sf::Packet& packet, const StartGamePacket& startGamePacket);
sf::Packet& operator >>(sf::Packet& packet, StartGamePacket& startGamePacket);