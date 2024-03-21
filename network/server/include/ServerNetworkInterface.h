#pragma once

#include "Packet.h"
#include "ClientId.h"

struct PacketData
{
	Packet* PacketReceived{};
	ClientId FromClient;
};

class ServerNetworkInterface
{
public:
	/**
	 * @brief Get the next packet to process
	 * Need to delete the packet after using it
	 * @return
	 */
	virtual PacketData PopPacket() = 0;

	/**
	 * @brief Send a packet to a specific client
	 * @param packet The packet to send
	 * @param clientIndex The index of the client to send the packet to
	 */
	virtual void SendPacket(Packet* packet, ClientId clientId) = 0;
};