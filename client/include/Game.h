#pragma once

#include "Packet.h"
#include "DeckType.h"

#include <SFML/Graphics.hpp>

#include <array>

enum class GameState
{
	NONE,
	MAIN_MENU,
	LOBBY,
	GAME
};

struct PlayerData
{
	std::string Name;
	std::size_t IconIndex = 20;
};

struct LobbyData
{
	bool IsHost{};
	bool WaitingForOpponent = true;
	DeckType DeckType = DeckType::Deck3x2;
	PlayerData Player1;
	PlayerData Player2;
};

struct CardData
{
	// If -1, then it's not known
	int IconIndex = -1;
	bool Flipped = false;
};

struct GameData
{
	// 50 cards in 5x10 grid
	std::array<CardData, 50> Cards;
	std::size_t PlayerScore{};
	std::size_t OpponentScore{};
	bool YourTurn{};

	void Reset()
	{
		PlayerScore = 0;
		OpponentScore = 0;
		YourTurn = false;

		for (auto& card : Cards)
		{
			card.IconIndex = -1;
			card.Flipped = false;
		}
	}
};

namespace Game
{
	inline static float HEIGHT = 1080.f;
	inline static float WIDTH = 1920.f;

	void Initialize();
	int StartLoop();
	sf::RenderWindow& GetWindow();
	void SetState(GameState state);
	void Exit();

	void SendPacket(Packet* packet);

	PlayerData& GetPlayer();
	LobbyData& GetLobby();
	GameData& GetGame();
}