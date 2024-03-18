#pragma once

#include "MyPackets.h"

namespace MyPackets
{
	class TurnPacket final : public Packet
	{
	public:
		TurnPacket() : Packet(static_cast<char>(MyPacketType::Turn)) {}
		explicit TurnPacket(bool yourTurn) : Packet(static_cast<char>(MyPacketType::Turn)), YourTurn(yourTurn) {}

		bool YourTurn{};

		[[nodiscard]] Packet* Clone() const override { return new TurnPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "TurnPacket"; }

		void Write(sf::Packet& packet) const override { packet << YourTurn; }
		void Read(sf::Packet& packet) override { packet >> YourTurn; }
	};
}