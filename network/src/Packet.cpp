#include "Packet.h"

Packet* Packet::FromType(PacketType type)
{
	switch (type)
	{
	case PacketType::LobbyInformation: return new LobbyInformationPacket();
	case PacketType::ChangeDeck: return new ChangeDeckPacket();
	case PacketType::JoinLobby: return new JoinLobbyPacket();
	case PacketType::LeaveLobby: return new LeaveLobbyPacket();
	case PacketType::StartGame: return new StartGamePacket();
	default: return new InvalidPacket();
	}
}

sf::Packet& operator <<(sf::Packet& packet, const Packet& packetType)
{
	packet << static_cast<sf::Uint8>(packetType.type);

	switch (packetType.type)
	{
	case PacketType::LobbyInformation:
	{
		const auto& joinLobbyPacket = dynamic_cast<const LobbyInformationPacket&>(packetType);
		packet << joinLobbyPacket.IsHost << joinLobbyPacket.WaitingForOpponent << joinLobbyPacket.Player1Name
			<< joinLobbyPacket.Player2Name << joinLobbyPacket.Player1Icon << joinLobbyPacket.Player2Icon << static_cast<sf::Uint8>(joinLobbyPacket.DeckType);
		break;
	}
	case PacketType::ChangeDeck:
	{
		const auto& changeDeckPacket = dynamic_cast<const ChangeDeckPacket&>(packetType);
		packet << static_cast<sf::Uint8>(changeDeckPacket.DeckType);
		break;
	}
	case PacketType::JoinLobby:
	{
		const auto& joinLobbyPacket = dynamic_cast<const JoinLobbyPacket&>(packetType);
		packet << joinLobbyPacket.Name << joinLobbyPacket.IconIndex;
		break;
	}
	case PacketType::LeaveLobby:
	{
		// No data to write
		break;
	}
	case PacketType::StartGame:
	{
		const auto& startGamePacket = dynamic_cast<const StartGamePacket&>(packetType);
		packet << startGamePacket.YourTurn;
		break;
	}
	default:
		break;
	}

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Packet& packetType)
{
	switch (packetType.type)
	{
	case PacketType::LobbyInformation:
	{
		auto* joinLobbyPacket = dynamic_cast<LobbyInformationPacket*>(&packetType);
		packet >> joinLobbyPacket->IsHost >> joinLobbyPacket->WaitingForOpponent >> joinLobbyPacket->Player1Name
			>> joinLobbyPacket->Player2Name >> joinLobbyPacket->Player1Icon >> joinLobbyPacket->Player2Icon;
		sf::Uint8 deckTypeUint;
		packet >> deckTypeUint;
		joinLobbyPacket->DeckType = static_cast<DeckType>(deckTypeUint);
		break;
	}
	case PacketType::ChangeDeck:
	{
		auto* changeDeckPacket = dynamic_cast<ChangeDeckPacket*>(&packetType);
		sf::Uint8 deckTypeUint;
		packet >> deckTypeUint;
		changeDeckPacket->DeckType = static_cast<DeckType>(deckTypeUint);
		break;
	}
	case PacketType::JoinLobby:
	{
		auto* joinLobbyPacket = dynamic_cast<JoinLobbyPacket*>(&packetType);
		packet >> joinLobbyPacket->Name >> joinLobbyPacket->IconIndex;
		break;
	}
	case PacketType::LeaveLobby:
	{
		// No data to read
		break;
	}
	case PacketType::StartGame:
	{
		auto* startGamePacket = dynamic_cast<StartGamePacket*>(&packetType);
		packet >> startGamePacket->YourTurn;
		break;
	}
	default:
		break;
	}

	return packet;
}