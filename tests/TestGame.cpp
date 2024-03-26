#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "GameManager.h"
#include "ClientNetworkInterface.h"
#include "ServerNetworkInterface.h"

#include <gtest/gtest.h>

class TestClientNetwork final : public ClientNetworkInterface
{
public:
	Packet* LastPacketReceived = nullptr;
	std::queue<Packet*> PacketsToSend;

	Packet* PopPacket() override
	{
		if (PacketsToSend.empty()) return nullptr;

		auto packet = PacketsToSend.front();
		PacketsToSend.pop();
		return packet;
	}

	void SendPacket(Packet* packet) override
	{
		LastPacketReceived = packet;
	}
};

inline static float HEIGHT = 1080.f;
inline static float WIDTH = 1920.f;

class TestServerNetwork final : public ServerNetworkInterface
{
public:
	PacketData LastPacketSent {};
	std::queue<PacketData> PacketsToProcess;
	std::queue<ClientId> DisconnectedClients;

	PacketData PopPacket() override
	{
		if (PacketsToProcess.empty()) return {};

		auto packet = PacketsToProcess.front();
		PacketsToProcess.pop();
		return packet;
	}

	void SendPacket(Packet* packet, const ClientId& clientId) override
	{
		LastPacketSent = { packet, clientId };
	}

	ClientId PopDisconnectedClient() override
	{
		if (DisconnectedClients.empty()) return {};

		auto client = DisconnectedClients.front();
		DisconnectedClients.pop();
		return client;
	}
};

TEST(TestGame, CompleteGame)
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();

	TestServerNetwork serverNetwork;
	Server server(serverNetwork);

	std::array<TestClientNetwork, 2> networkClientManagers;
	std::array<GameManager, 2> gameManagers = {
			GameManager(),
			GameManager()
	};
	std::array<Game, 2> games = {
			Game(gameManagers[0], networkClientManagers[0], WIDTH, HEIGHT),
			Game(gameManagers[1], networkClientManagers[1], WIDTH, HEIGHT)
	};
	std::array<ClientId, 2> clientIds = {0, 1};

	const auto& p1Game = gameManagers[0].GetGame();
	const auto& p1Lobby = gameManagers[0].GetLobby();
	const auto& p1Player = gameManagers[0].GetPlayer();
	const auto& p2Game = gameManagers[1].GetGame();
	const auto& p2Lobby = gameManagers[1].GetLobby();
	const auto& p2Player = gameManagers[1].GetPlayer();

	gameManagers[0].SetUsername("Player1");
	gameManagers[0].SetPlayerIcon(IconType::Icon5);
	gameManagers[1].SetUsername("Player2");
	gameManagers[1].SetPlayerIcon(IconType::Icon3);

	// 1. Each player join the lobby
	games[0].SetState(GameState::LOBBY);

	ASSERT_EQ(p1Lobby.IsHost, true);
	ASSERT_EQ(p1Lobby.WaitingForOpponent, true);
	ASSERT_EQ(networkClientManagers[0].LastPacketReceived->Type, static_cast<char>(MyPackets::MyPacketType::JoinLobby));

	serverNetwork.PacketsToProcess.push({ networkClientManagers[0].LastPacketReceived, clientIds[0] });


}