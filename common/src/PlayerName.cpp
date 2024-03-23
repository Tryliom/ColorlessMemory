#include "PlayerName.h"

sf::Packet& operator <<(sf::Packet& packet, const PlayerName& playerName)
{
	for (const auto& c : playerName.Name)
	{
		packet << c;
	}
	return packet;
}


sf::Packet& operator >>(sf::Packet& packet, PlayerName& playerName)
{
	for (auto& c : playerName.Name)
	{
		packet >> &c;
	}
	return packet;
}
