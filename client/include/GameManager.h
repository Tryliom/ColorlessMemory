#pragma once

#include "Constants.h"
#include "PlayerName.h"
#include "Packet.h"
#include "MyPackets/JoinLobbyPacket.h"

#include <string>

struct PlayerData
{
	PlayerName Name;
	IconType IconIndex{};
};

struct Lobby
{
	bool IsHost{};
	bool WaitingForOpponent = true;
	DeckType DeckType = DeckType::Deck3x2;
	PlayerData Player1;
	PlayerData Player2;
};

struct GameData
{
	DeckType DeckType = DeckType::Deck3x2;
	CardIndex CardIndex1{ UNKNOWN_CARD_INDEX };
	CardIndex CardIndex2{ UNKNOWN_CARD_INDEX };
	char Player1Score{};
	char Player2Score{};
	bool IsFirstPlayer{};
	bool YourTurn{};

	void Reset(const Lobby& lobby)
	{
		DeckType = lobby.DeckType;
		Player1Score = 0;
		Player2Score = 0;
		IsFirstPlayer = lobby.IsHost;
	}
};

class GameManager
{
 public:
	GameManager() = default;

 private:
	PlayerData _player;
	Lobby _lobby;
	GameData _game;

 public:
	void OnPacketReceived(Packet& packet);

	void JoinLobby();

	void SetUsername(const std::string& username);
	void IncreaseScore(char playerIndex);
	/**
	 * @brief Choose a card if it's the player's turn and the card is not already selected
	 * @param cardIndexInDeck
	 * @return True if the card was selected, false otherwise
	 */
	bool ChooseACard(CardIndex cardIndexInDeck);
	/**
	 * @brief End the player's turn by putting UNKNOWN_CARD_INDEX in CardIndex1 and CardIndex2
	 */
	void EndTurn();

	[[nodiscard]] MyPackets::JoinLobbyPacket* ToJoinLobbyPacket() const;

	[[nodiscard]] const PlayerData& GetPlayer() const;
	[[nodiscard]] const Lobby& GetLobby() const;
	[[nodiscard]] const GameData& GetGame() const;
};