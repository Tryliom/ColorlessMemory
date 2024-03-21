#pragma once

#include "Packet.h"
#include "DeckType.h"
#include "Constants.h"

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
	int CardIndex1 {UNKNOWN_CARD_INDEX};
	int CardIndex2 {UNKNOWN_CARD_INDEX};
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

namespace Game
{
	inline static float HEIGHT = 1080.f;
	inline static float WIDTH = 1920.f;

	void Initialize();
	void CheckInputs(const sf::Event& event);
	void Update(sf::Time elapsed);
	void SetState(GameState state);
	void Exit();
	void Render(sf::RenderTarget& target);

	void SendPacket(Packet* packet);

	PlayerData& GetPlayer();
	Lobby& GetLobby();
	GameData& GetGame();
}