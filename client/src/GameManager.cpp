#include "GameManager.h"
#include "MyPackets.h"
#include "MyPackets/LobbyInformationPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/TurnPacket.h"
#include "MyPackets/CardInformationPacket.h"

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::LobbyInformation))
	{
		auto joinLobbyPacket = *packet.As<MyPackets::LobbyInformationPacket>();

		_lobby.IsHost = joinLobbyPacket.IsHost;
		_lobby.WaitingForOpponent = joinLobbyPacket.WaitingForOpponent;
		_lobby.Player1.Name = joinLobbyPacket.Player1Name;
		_lobby.Player1.IconIndex = joinLobbyPacket.Player1Icon;
		_lobby.Player2.Name = joinLobbyPacket.Player2Name;
		_lobby.Player2.IconIndex = joinLobbyPacket.Player2Icon;
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_lobby.DeckType = startGamePacket.ChosenDeckType;
		_game.Reset(_lobby);
		_game.YourTurn = startGamePacket.YourTurn;
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::Turn))
	{
		auto turnPacket = *packet.As<MyPackets::TurnPacket>();

		_game.YourTurn = turnPacket.YourTurn;
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::CardInformation))
	{
		auto cardInformationPacket = *packet.As<MyPackets::CardInformationPacket>();

		const auto cardIndex = cardInformationPacket.CardIndexInDeck;

		if (_game.CardIndex1 == UNKNOWN_CARD_INDEX || _game.CardIndex1 == cardIndex)
		{
			_game.CardIndex1 = cardIndex;
		}
		else if (_game.CardIndex2 == UNKNOWN_CARD_INDEX || _game.CardIndex2 == cardIndex)
		{
			_game.CardIndex2 = cardIndex;
		}
	}
}

void GameManager::SetUsername(const std::string& username)
{
	_player.Name = PlayerName(username);
}

void GameManager::SetPlayerIcon(IconType iconIndex)
{
	_player.IconIndex = iconIndex;
}

void GameManager::IncreaseScore(char playerIndex)
{
	if (playerIndex == 0)
	{
		_game.Player1Score++;
	}
	else
	{
		_game.Player2Score++;
	}
}

bool GameManager::ChooseACard(CardIndex cardIndexInDeck)
{
	if (!_game.YourTurn || _game.CardIndex1 != UNKNOWN_CARD_INDEX && _game.CardIndex2 != UNKNOWN_CARD_INDEX) return false;
	if (_game.CardIndex1 == cardIndexInDeck || _game.CardIndex2 == cardIndexInDeck) return false;

	if (_game.CardIndex1 == UNKNOWN_CARD_INDEX)
	{
		_game.CardIndex1 = cardIndexInDeck;
	}
	else if (_game.CardIndex2 == UNKNOWN_CARD_INDEX)
	{
		_game.CardIndex2 = cardIndexInDeck;
	}

	return true;
}

void GameManager::ChangeDeck(DeckType deckType)
{
	_lobby.DeckType = deckType;
}

void GameManager::EndTurn()
{
	_game.CardIndex1 = UNKNOWN_CARD_INDEX;
	_game.CardIndex2 = UNKNOWN_CARD_INDEX;
}

void GameManager::JoinLobby()
{
	_lobby.IsHost = true;
	_lobby.WaitingForOpponent = true;
}

MyPackets::JoinLobbyPacket* GameManager::ToJoinLobbyPacket() const
{
	return new MyPackets::JoinLobbyPacket(_player.Name.AsString(), _player.IconIndex);
}

const GameData::Player& GameManager::GetPlayer() const
{
	return _player;
}

const GameData::Lobby& GameManager::GetLobby() const
{
	return _lobby;
}

const GameData::Game& GameManager::GetGame() const
{
	return _game;
}