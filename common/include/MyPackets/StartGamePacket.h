#pragma once

#include "MyPackets.h"
#include "Constants.h"

namespace MyPackets
{
	class StartGamePacket final : public Packet
	{
	public:
		StartGamePacket() : Packet(static_cast<char>(MyPacketType::StartGame)) {}
		explicit StartGamePacket(DeckType deckType, bool yourTurn) : Packet(static_cast<char>(MyPacketType::StartGame)), ChosenDeckType(deckType), YourTurn(yourTurn) {}

		DeckType ChosenDeckType{};
		bool YourTurn{};

		[[nodiscard]] Packet* Clone() const override { return new StartGamePacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "StartGamePacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(ChosenDeckType) << YourTurn;
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 deckTypeUint;
			packet >> deckTypeUint >> YourTurn;
			ChosenDeckType = static_cast<DeckType>(deckTypeUint);
		}
	};
}