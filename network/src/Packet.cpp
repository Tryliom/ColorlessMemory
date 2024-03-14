#include "Packet.h"

std::string PacketTypeToString(PacketType type)
{
	switch (type)
	{
	case PacketType::LobbyInformation: return "LobbyInformation";
	case PacketType::ChangeDeck: return "ChangeDeck";
	case PacketType::JoinLobby: return "JoinLobby";
	case PacketType::LeaveLobby: return "LeaveLobby";
	case PacketType::StartGame: return "StartGame";
	case PacketType::Turn: return "Turn";
	case PacketType::CardInformation: return "CardInformation";
	case PacketType::LeaveGame: return "LeaveGame";
	default: return "Invalid";
	}
}

Packet* Packet::FromType(PacketType type)
{
	switch (type)
	{
	case PacketType::LobbyInformation: return new LobbyInformationPacket();
	case PacketType::ChangeDeck: return new ChangeDeckPacket();
	case PacketType::JoinLobby: return new JoinLobbyPacket();
	case PacketType::LeaveLobby: return new LeaveLobbyPacket();
	case PacketType::StartGame: return new StartGamePacket();
	case PacketType::Turn: return new TurnPacket();
	case PacketType::CardInformation: return new CardInformationPacket();
	case PacketType::LeaveGame: return new LeaveGamePacket();
	default: return new InvalidPacket();
	}
}

sf::Packet& operator <<(sf::Packet& packet, const Packet& myPacket)
{
	packet << static_cast<sf::Uint8>(myPacket.type);

	switch (myPacket.type)
	{
	case PacketType::LobbyInformation:
	{
		const auto& joinLobbyPacket = dynamic_cast<const LobbyInformationPacket&>(myPacket);
		packet << joinLobbyPacket.IsHost << joinLobbyPacket.WaitingForOpponent << joinLobbyPacket.Player1Name
			<< joinLobbyPacket.Player2Name << joinLobbyPacket.Player1Icon << joinLobbyPacket.Player2Icon << static_cast<sf::Uint8>(joinLobbyPacket.ChosenDeckType);
		break;
	}
	case PacketType::ChangeDeck:
	{
		const auto& changeDeckPacket = dynamic_cast<const ChangeDeckPacket&>(myPacket);
		packet << static_cast<sf::Uint8>(changeDeckPacket.ChosenDeckType);
		break;
	}
	case PacketType::JoinLobby:
	{
		const auto& joinLobbyPacket = dynamic_cast<const JoinLobbyPacket&>(myPacket);
		packet << joinLobbyPacket.Name << joinLobbyPacket.IconIndex;
		break;
	}
	case PacketType::StartGame:
	{
		const auto& startGamePacket = dynamic_cast<const StartGamePacket&>(myPacket);
		packet << static_cast<sf::Uint8>(startGamePacket.ChosenDeckType) << startGamePacket.YourTurn;
		break;
	}
	case PacketType::Turn:
	{
		const auto& turnPacket = dynamic_cast<const TurnPacket&>(myPacket);
		packet << turnPacket.YourTurn;
		break;
	}
	case PacketType::CardInformation:
	{
		const auto& cardInformationPacket = dynamic_cast<const CardInformationPacket&>(myPacket);
		packet << cardInformationPacket.CardIndex << cardInformationPacket.IconIndex;
		break;
	}
	case PacketType::LeaveLobby:
	case PacketType::LeaveGame:
	{
		// No data to write in the packet to send
		break;
	}
	default: break;
	}

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Packet& myPacket)
{
	switch (myPacket.type)
	{
	case PacketType::LobbyInformation:
	{
		auto* joinLobbyPacket = dynamic_cast<LobbyInformationPacket*>(&myPacket);
		packet >> joinLobbyPacket->IsHost >> joinLobbyPacket->WaitingForOpponent >> joinLobbyPacket->Player1Name
			>> joinLobbyPacket->Player2Name >> joinLobbyPacket->Player1Icon >> joinLobbyPacket->Player2Icon;
		sf::Uint8 deckTypeUint;
		packet >> deckTypeUint;
		joinLobbyPacket->ChosenDeckType = static_cast<DeckType>(deckTypeUint);
		break;
	}
	case PacketType::ChangeDeck:
	{
		auto* changeDeckPacket = dynamic_cast<ChangeDeckPacket*>(&myPacket);
		sf::Uint8 deckTypeUint;
		packet >> deckTypeUint;
		changeDeckPacket->ChosenDeckType = static_cast<DeckType>(deckTypeUint);
		break;
	}
	case PacketType::JoinLobby:
	{
		auto* joinLobbyPacket = dynamic_cast<JoinLobbyPacket*>(&myPacket);
		packet >> joinLobbyPacket->Name >> joinLobbyPacket->IconIndex;
		break;
	}
	case PacketType::StartGame:
	{
		auto* startGamePacket = dynamic_cast<StartGamePacket*>(&myPacket);
		sf::Uint8 deckTypeUint;
		packet >> deckTypeUint >> startGamePacket->YourTurn;
		startGamePacket->ChosenDeckType = static_cast<DeckType>(deckTypeUint);
		break;
	}
	case PacketType::Turn:
	{
		auto* turnPacket = dynamic_cast<TurnPacket*>(&myPacket);
		packet >> turnPacket->YourTurn;
		break;
	}
	case PacketType::CardInformation:
	{
		auto* cardInformationPacket = dynamic_cast<CardInformationPacket*>(&myPacket);
		packet >> cardInformationPacket->CardIndex >> cardInformationPacket->IconIndex;
		break;
	}
	case PacketType::LeaveLobby:
	case PacketType::LeaveGame:
	{
		// No data to read
		break;
	}
	default:
		break;
	}

	return packet;
}