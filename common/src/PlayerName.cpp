#include "PlayerName.h"

sf::Packet& operator <<(sf::Packet& packet, const PlayerName& playerName)
{
	for (const auto& c : playerName.Name)
	{
		packet << static_cast<sf::Uint8>(c);
	}
	return packet;
}


sf::Packet& operator >>(sf::Packet& packet, PlayerName& playerName)
{
	for (auto& c : playerName.Name)
	{
		sf::Uint8 temp;
		packet >> temp;
		c = static_cast<char>(temp);
	}
	return packet;
}
