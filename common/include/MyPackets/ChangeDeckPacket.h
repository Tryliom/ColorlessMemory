#pragma once

#include "MyPackets.h"
#include "Constants.h"

namespace MyPackets
{
	class ChangeDeckPacket final : public Packet
	{
	public:
		ChangeDeckPacket() : Packet(static_cast<char>(MyPacketType::ChangeDeck)) {}

		explicit ChangeDeckPacket(DeckType deckType) : Packet(static_cast<char>(MyPacketType::ChangeDeck)), ChosenDeckType(deckType) {}

		DeckType ChosenDeckType{};

		[[nodiscard]] Packet* Clone() const override { return new ChangeDeckPacket(); }
		[[nodiscard]] std::string ToString() const override { return "ChangeDeckPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<char>(ChosenDeckType);
		}

		void Read(sf::Packet& packet) override
		{
			char deckType;
			packet >> &deckType;
			ChosenDeckType = static_cast<DeckType>(deckType);
		}
	};
}