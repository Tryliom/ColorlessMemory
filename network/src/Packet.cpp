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
		packet << joinLobbyPacket;
		break;
	}
	case PacketType::ChangeDeck:
	{
		const auto& changeDeckPacket = dynamic_cast<const ChangeDeckPacket&>(packetType);
		packet << changeDeckPacket;
		break;
	}
	case PacketType::JoinLobby:
	{
		const auto& joinLobbyPacket = dynamic_cast<const JoinLobbyPacket&>(packetType);
		packet << joinLobbyPacket;
		break;
	}
	case PacketType::LeaveLobby:
	{
		const auto& leaveLobbyPacket = dynamic_cast<const LeaveLobbyPacket&>(packetType);
		packet << leaveLobbyPacket;
		break;
	}
	case PacketType::StartGame:
	{
		const auto& startGamePacket = dynamic_cast<const StartGamePacket&>(packetType);
		packet << startGamePacket;
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
		packet >> *joinLobbyPacket;
		break;
	}
	case PacketType::ChangeDeck:
	{
		auto* changeDeckPacket = dynamic_cast<ChangeDeckPacket*>(&packetType);
		packet >> *changeDeckPacket;
		break;
	}
	case PacketType::JoinLobby:
	{
		auto* joinLobbyPacket = dynamic_cast<JoinLobbyPacket*>(&packetType);
		packet >> *joinLobbyPacket;
		break;
	}
	case PacketType::LeaveLobby:
	{
		auto* leaveLobbyPacket = dynamic_cast<LeaveLobbyPacket*>(&packetType);
		packet >> *leaveLobbyPacket;
		break;
	}
	case PacketType::StartGame:
	{
		auto* startGamePacket = dynamic_cast<StartGamePacket*>(&packetType);
		packet >> *startGamePacket;
		break;
	}
	default:
		break;
	}

	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const LobbyInformationPacket& joinLobbyPacket)
{
	return packet << joinLobbyPacket.IsHost << joinLobbyPacket.WaitingForOpponent << joinLobbyPacket.Player1Name
		<< joinLobbyPacket.Player2Name << joinLobbyPacket.Player1Icon << joinLobbyPacket.Player2Icon << static_cast<sf::Uint8>(joinLobbyPacket.DeckType);
}

sf::Packet& operator >>(sf::Packet& packet, LobbyInformationPacket& joinLobbyPacket)
{
	packet >> joinLobbyPacket.IsHost >> joinLobbyPacket.WaitingForOpponent >> joinLobbyPacket.Player1Name
		>> joinLobbyPacket.Player2Name >> joinLobbyPacket.Player1Icon >> joinLobbyPacket.Player2Icon;
	sf::Uint8 deckTypeUint;
	packet >> deckTypeUint;
	joinLobbyPacket.DeckType = static_cast<DeckType>(deckTypeUint);

	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const ChangeDeckPacket& changeDeckPacket)
{
	return packet << static_cast<sf::Uint8>(changeDeckPacket.DeckType);
}

sf::Packet& operator >>(sf::Packet& packet, ChangeDeckPacket& changeDeckPacket)
{
	sf::Uint8 deckTypeUint;
	packet >> deckTypeUint;
	changeDeckPacket.DeckType = static_cast<DeckType>(deckTypeUint);

	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const JoinLobbyPacket& joinLobbyPacket)
{
	return packet << joinLobbyPacket.Name << joinLobbyPacket.IconIndex;
}

sf::Packet& operator >>(sf::Packet& packet, JoinLobbyPacket& joinLobbyPacket)
{
	return packet >> joinLobbyPacket.Name >> joinLobbyPacket.IconIndex;
}

sf::Packet& operator <<(sf::Packet& packet, const LeaveLobbyPacket& leaveLobbyPacket)
{
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, LeaveLobbyPacket& leaveLobbyPacket)
{
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const StartGamePacket& startGamePacket)
{
	return packet << startGamePacket.YourTurn;
}

sf::Packet& operator >>(sf::Packet& packet, StartGamePacket& startGamePacket)
{
	return packet >> startGamePacket.YourTurn;
}