#include "NetworkServerManager.h"

#include "Logger.h"

#include <thread>
#include <utility>

NetworkServerManager::~NetworkServerManager()
{
	Running = false;
	_listener.close();
}

bool NetworkServerManager::Bind(unsigned short port)
{
	return _listener.listen(port) == sf::Socket::Done;
}

void NetworkServerManager::OnPacketReceived(std::function<void(sf::TcpSocket*, Packet*)> onPacketReceived)
{
	this->_onPacketReceived = std::move(onPacketReceived);
}

void NetworkServerManager::OnDisconnect(std::function<void(sf::TcpSocket*)> onDisconnect)
{
	this->_onDisconnect = std::move(onDisconnect);
}

void NetworkServerManager::StartThreads()
{
	std::thread clientAcceptor = std::thread([this]() {
		while (Running)
		{
			// Accept a new connection
			std::scoped_lock lock(_mutex);
			auto* socket = new sf::TcpSocket();

			if (_listener.accept(*socket) != sf::Socket::Done)
			{
				LOG_ERROR("Could not accept connection");
			}
			else
			{
				auto index = _clients.AddClient(socket);
				auto& client = *_clients[index];

				LOG("Client connected: " << client.socket->getRemoteAddress() << ':' << client.socket->getRemotePort());

				// Start a new thread to receive packets from the client
				std::thread clientReceiver = std::thread([this, index]() {ReceivePacketFromClient(index);});
				clientReceiver.detach();
			}
		}
	});
	clientAcceptor.detach();

	std::thread packetSender = std::thread([&] {
		while (Running)
		{
			if (_clients.isEmpty()) continue;

			_clients.CheckPacketToBeSent();
		}
	});
	packetSender.detach();
}

void NetworkServerManager::ReceivePacketFromClient(std::size_t clientIndex)
{
	auto* client = _clients[clientIndex];
	sf::TcpSocket* socket = client->socket;
	while (true)
	{
		// Receive a message from the client
		Packet* packet = PacketManager::ReceivePacket(*socket);

		if (packet->Type == PacketType::Invalid) break;
		if (_onPacketReceived) _onPacketReceived(socket, packet);

		delete packet;
	}

	if (_onDisconnect) _onDisconnect(socket);
	_clients.RemoveClient(clientIndex);
}