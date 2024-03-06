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
	std::vector<CardData> Cards = std::vector<CardData>();
	std::size_t Player1Score{};
	std::size_t Player2Score{};
	bool IsFirstPlayer{};
	bool YourTurn{};

	void Reset(const LobbyData& lobby)
	{
		Player1Score = 0;
		Player2Score = 0;
		IsFirstPlayer = lobby.IsHost;

		Cards.clear();

		auto cards = 0;

		switch (lobby.DeckType)
		{
		case DeckType::Deck3x2:
			cards = 3 * 2;
			break;
		case DeckType::Deck7x2:
			cards = 7 * 2;
			break;
		case DeckType::Deck6x5:
			cards = 6 * 5;
			break;
		case DeckType::Deck7x6:
			cards = 7 * 6;
			break;
		case DeckType::Deck10x5:
			cards = 10 * 5;
			break;
		}

		for (std::size_t i = 0; i < cards; ++i)
		{
			Cards.emplace_back();
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