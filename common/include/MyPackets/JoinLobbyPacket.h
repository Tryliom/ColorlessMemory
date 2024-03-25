#pragma once

#include "MyPackets.h"
#include "PlayerName.h"
#include "Constants.h"

namespace MyPackets
{
	class JoinLobbyPacket final : public Packet
	{
	public:
		JoinLobbyPacket() : Packet(static_cast<char>(MyPacketType::JoinLobby)) {}
		explicit JoinLobbyPacket(std::string_view name, IconType iconIndex) : Packet(static_cast<char>(MyPacketType::JoinLobby)),
			Name(name), IconIndex(iconIndex) {}

		PlayerName Name;
		IconType IconIndex{};

		[[nodiscard]] Packet* Clone() const override { return new JoinLobbyPacket(); }
		[[nodiscard]] std::string ToString() const override { return "JoinLobbyPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << Name;
			packet << static_cast<sf::Uint8>(IconIndex);
		}

		void Read(sf::Packet& packet) override
		{
			packet >> Name;

			sf::Uint8 iconIndex;
			packet >> iconIndex;

			IconIndex = static_cast<IconType>(iconIndex);
		}
	};
}