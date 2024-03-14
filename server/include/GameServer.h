#pragma once

#include "NetworkServerManager.h"
#include "DeckType.h"

#include <SFML/Network.hpp>

#include <array>

struct Lobby
{
	sf::TcpSocket* player1;
	sf::TcpSocket* player2;
	std::string player1Name;
	std::string player2Name;
	std::size_t player1Icon;
	std::size_t player2Icon;
	DeckType deckType;

	[[nodiscard]] LobbyInformationPacket* ToPacket(bool isHost) const
	{
		return new LobbyInformationPacket(isHost, player1 == nullptr || player2 == nullptr, player1Name, player2Name,
			player1Icon, player2Icon, deckType);
	}

	void Reset()
	{
		player1 = nullptr;
		player2 = nullptr;
		player1Name.clear();
		player2Name.clear();
		player1Icon = 0;
		player2Icon = 0;
		deckType = DeckType::Deck3x2;
	}
};

struct Game
{
	sf::TcpSocket* player1{};
	sf::TcpSocket* player2{};

	std::vector<std::size_t> cards;

	std::size_t turn{};
	std::array<int, 2> selectedCards{};

	std::size_t totalScore {};

	explicit Game(const Lobby& lobbyData);

	[[nodiscard]] bool HasSelectedTwoCards() const
	{
		return selectedCards[0] != -1 && selectedCards[1] != -1;
	}

	void SelectCard(int index)
	{
		if (selectedCards[0] == -1)
		{
			selectedCards[0] = index;
		}
		else if (selectedCards[1] == -1)
		{
			selectedCards[1] = index;
		}
	}

	void UnselectCards()
	{
		selectedCards[0] = -1;
		selectedCards[1] = -1;
	}

	[[nodiscard]] bool IsGameOver() const
	{
		return totalScore == cards.size() / 2;
	}
};

namespace GameServer
{
	void Initialize();
	void StartLoop();
};