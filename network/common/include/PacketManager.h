#pragma once

#include "Packet.h"

namespace PacketManager
{
	bool SendPacket(sf::TcpSocket& socket, Packet* packet);

	/**
	 * @brief Receive a packet from a socket, it use GetPacketType to get the type of the packet
	 * @param socket The socket to receive the packet from
	 * @param packet The packet to store the data
	 * @return The packet
	 */
	Packet* ReceivePacket(sf::TcpSocket& socket);

	sf::Packet* CreatePacket(Packet* packet);

	//TODO: Use only funcs: sf::Packet* FromPacket(Packet* packet); and Packet* ToPacket(sf::Packet* packet);

	/**
	 * @brief Register a new packet type
	 * @param packet The packet to register
	 */
	void RegisterPacketType(Packet* packet);
}