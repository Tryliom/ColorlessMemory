#include "Packet.h"

sf::Packet& operator <<(sf::Packet& packet, const Packet& myPacket)
{
	packet << myPacket.Type;

	myPacket.Write(packet);

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Packet& myPacket)
{
	myPacket.Read(packet);

	return packet;
}