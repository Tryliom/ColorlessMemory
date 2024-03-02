#pragma once

#include "packet.h"

namespace PacketManager
{
	bool SendPacket(sf::TcpSocket& socket, const ConnectPacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const DisconnectPacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const MessagePacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const AcknowledgementPacket& packet);

	/**
	 * @brief Get the packet type, it will get the first byte of the packet and cast it to PacketType
	 * It's erased from the packet, so don't use it twice
	 * @param packet The packet to get the type from
	 * @return The type of the packet
	 */
	PacketType GetPacketType(sf::Packet& packet);
	/**
	 * @brief Receive a packet from a socket, it use GetPacketType to get the type of the packet
	 * @param socket The socket to receive the packet from
	 * @param packet The packet to store the data
	 * @return The type of the packet
	 */
	PacketType ReceivePacket(sf::TcpSocket& socket, sf::Packet& packet);

	ConnectPacket GetConnectPacket(sf::Packet& packet);
	DisconnectPacket GetDisconnectPacket(sf::Packet& packet);
	MessagePacket GetMessagePacket(sf::Packet& packet);

	sf::Packet* CreatePacket(const ConnectPacket& packet);
	sf::Packet* CreatePacket(const DisconnectPacket& packet);
	sf::Packet* CreatePacket(const MessagePacket& packet);
	sf::Packet* CreatePacket(const AcknowledgementPacket& packet);
}