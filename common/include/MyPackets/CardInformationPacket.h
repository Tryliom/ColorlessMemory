#pragma once

#include "MyPackets.h"
#include "Constants.h"

namespace MyPackets
{
	class CardInformationPacket final : public Packet
	{
	public:
		CardInformationPacket() : Packet(static_cast<char>(MyPacketType::CardInformation)) {}
		explicit CardInformationPacket(CardIndex cardIndexInDeck, CardIconIndex iconIndex) : Packet(static_cast<char>(MyPacketType::CardInformation)), CardIndexInDeck(cardIndexInDeck), IconIndex(iconIndex) {}

		CardIndex CardIndexInDeck {UNKNOWN_CARD_INDEX};
		CardIconIndex IconIndex {UNKNOWN_ICON_INDEX};

		[[nodiscard]] Packet* Clone() const override { return new CardInformationPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "CardInformationPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(CardIndexInDeck.Index) << static_cast<sf::Uint8>(IconIndex.Index);
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 cardIndexUint, iconIndexUint;
			packet >> cardIndexUint >> iconIndexUint;

			CardIndexInDeck = CardIndex(static_cast<char>(cardIndexUint));
			IconIndex = CardIconIndex(static_cast<char>(iconIndexUint));
		}
	};

}