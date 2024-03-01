#pragma once

#include "packet.h"

namespace PacketManager
{
	bool SendPacket(sf::TcpSocket& socket, const ConnectPacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const DisconnectPacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const MessagePacket& packet);
	bool SendPacket(sf::TcpSocket& socket, const AcknowledgementPacket& packet);

	PacketType GetPacketType(sf::Packet& packet);
	PacketType ReceivePacket(sf::TcpSocket& socket, sf::Packet& packet);

	ConnectPacket GetConnectPacket(sf::Packet& packet);
	DisconnectPacket GetDisconnectPacket(sf::Packet& packet);
	MessagePacket GetMessagePacket(sf::Packet& packet);

	sf::Packet* CreatePacket(const ConnectPacket& packet);
	sf::Packet* CreatePacket(const DisconnectPacket& packet);
	sf::Packet* CreatePacket(const MessagePacket& packet);
	sf::Packet* CreatePacket(const AcknowledgementPacket& packet);
}