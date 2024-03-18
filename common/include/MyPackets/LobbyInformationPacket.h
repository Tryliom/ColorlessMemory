#pragma once

#include "MyPackets.h"
#include "Constants.h"
#include "PlayerName.h"

namespace MyPackets
{
	class LobbyInformationPacket final : public Packet
	{
	public:
		LobbyInformationPacket() : Packet(static_cast<char>(MyPacketType::LobbyInformation)) {}

		LobbyInformationPacket(bool isHost, bool waitingForOpponent, PlayerName player1Name, PlayerName player2Name,
				IconType player1Icon, IconType player2Icon, DeckType deckType)
				: Packet(static_cast<char>(MyPacketType::LobbyInformation))
		{
			IsHost = isHost;
			WaitingForOpponent = waitingForOpponent;
			Player1Name = std::move(player1Name);
			Player2Name = std::move(player2Name);
			Player1Icon = player1Icon;
			Player2Icon = player2Icon;
			ChosenDeckType = deckType;
		}

		PlayerName Player1Name{};
		PlayerName Player2Name{};
		IconType Player1Icon{};
		IconType Player2Icon{};
		DeckType ChosenDeckType{};
		bool IsHost{};
		bool WaitingForOpponent{};

		[[nodiscard]] Packet* Clone() const override { return new LobbyInformationPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "LobbyInformationPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << IsHost << WaitingForOpponent << Player1Name << Player2Name << static_cast<sf::Uint8>(Player1Icon)
				   << static_cast<sf::Uint8>(Player2Icon) << static_cast<sf::Uint8>(ChosenDeckType);
		}

		void Read(sf::Packet& packet) override
		{
			packet >> IsHost >> WaitingForOpponent >> Player1Name >> Player2Name;
			sf::Uint8 player1IconUint, player2IconUint, chosenDeckTypeUint;
			packet >> player1IconUint >> player2IconUint >> chosenDeckTypeUint;
			Player1Icon = static_cast<IconType>(player1IconUint);
			Player2Icon = static_cast<IconType>(player2IconUint);
			ChosenDeckType = static_cast<DeckType>(chosenDeckTypeUint);
		}
	};
}