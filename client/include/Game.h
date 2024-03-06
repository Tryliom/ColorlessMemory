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

struct GameData
{
	DeckType DeckType = DeckType::Deck3x2;
	std::size_t Player1Score{};
	std::size_t Player2Score{};
	bool IsFirstPlayer{};
	bool YourTurn{};

	void Reset(const LobbyData& lobby)
	{
		DeckType = lobby.DeckType;
		Player1Score = 0;
		Player2Score = 0;
		IsFirstPlayer = lobby.IsHost;
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