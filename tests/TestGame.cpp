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
	Packet* LastPacketSent = nullptr;
	std::queue<Packet*> PacketsToProcess;

	Packet* PopPacket() override
	{
		if (PacketsToProcess.empty()) return nullptr;

		auto packet = PacketsToProcess.front();
		PacketsToProcess.pop();
		return packet;
	}

	void SendPacket(Packet* packet) override
	{
		LastPacketSent = packet;
	}
};

inline static float HEIGHT = 1080.f;
inline static float WIDTH = 1920.f;

class TestServerNetwork final : public ServerNetworkInterface
{
public:
	std::vector<PacketData> PacketsSent; //TODO: Replace last packet sent with this
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
	const PlayerName p1Name = PlayerName("Player1");
	const PlayerName p2Name = PlayerName("Player2");
	const IconType p1Icon = IconType::Icon5;
	const IconType p2Icon = IconType::Icon3;

	const auto& p1Game = gameManagers[0].GetGame();
	const auto& p1Lobby = gameManagers[0].GetLobby();
	const auto& p1Player = gameManagers[0].GetPlayer();
	const auto& p2Game = gameManagers[1].GetGame();
	const auto& p2Lobby = gameManagers[1].GetLobby();
	const auto& p2Player = gameManagers[1].GetPlayer();

	const auto updateTime = sf::milliseconds(10000);

	gameManagers[0].SetUsername(p1Name.AsString());
	gameManagers[0].SetPlayerIcon(p1Icon);
	gameManagers[1].SetUsername(p2Name.AsString());
	gameManagers[1].SetPlayerIcon(p2Icon);

	// 1. Each player join the lobby
	// 1.1 Player 1 joins the lobby
	{
		games[0].SetState(GameState::LOBBY);

		ASSERT_EQ(p1Lobby.IsHost, true);
		ASSERT_EQ(p1Lobby.WaitingForOpponent, true);
		ASSERT_EQ(networkClientManagers[0].LastPacketSent->Type, static_cast<char>(MyPackets::MyPacketType::JoinLobby));

		serverNetwork.PacketsToProcess.push({ networkClientManagers[0].LastPacketSent, clientIds[0] });

		server.Update();

		ASSERT_EQ(serverNetwork.LastPacketSent.Packet->Type, static_cast<char>(MyPackets::MyPacketType::LobbyInformation));

		networkClientManagers[0].PacketsToProcess.push(serverNetwork.LastPacketSent.Packet);

		games[0].Update(updateTime, { 0, 0 });

		const auto lobbyInfoPacket = serverNetwork.LastPacketSent.Packet->As<MyPackets::LobbyInformationPacket>();

		ASSERT_EQ(lobbyInfoPacket->IsHost, p1Lobby.IsHost);
		ASSERT_EQ(lobbyInfoPacket->WaitingForOpponent, p1Lobby.WaitingForOpponent);
		ASSERT_EQ(lobbyInfoPacket->Player1Name.AsString(), p1Lobby.Player1.Name.AsString());
		ASSERT_EQ(lobbyInfoPacket->Player1Icon, p1Lobby.Player1.IconIndex);
		ASSERT_EQ(lobbyInfoPacket->Player2Name.AsString(), p1Lobby.Player2.Name.AsString());
		ASSERT_EQ(lobbyInfoPacket->Player2Icon, p1Lobby.Player2.IconIndex);
		ASSERT_EQ(lobbyInfoPacket->ChosenDeckType, p1Lobby.DeckType);
	}

	// 1.2 Player 2 joins the lobby
	{
		games[1].SetState(GameState::LOBBY);

		ASSERT_EQ(p2Lobby.IsHost, false);
		ASSERT_EQ(p2Lobby.WaitingForOpponent, false);
		ASSERT_EQ(networkClientManagers[1].LastPacketSent->Type, static_cast<char>(MyPackets::MyPacketType::JoinLobby));

		serverNetwork.PacketsToProcess.push({ networkClientManagers[1].LastPacketSent, clientIds[1] });

		server.Update();

		//TODO: Replace last packet sent with PacketsSent

		ASSERT_EQ(serverNetwork.LastPacketSent.Packet->Type, static_cast<char>(MyPackets::MyPacketType::LobbyInformation));

		networkClientManagers[1].PacketsToProcess.push(serverNetwork.LastPacketSent.Packet);

		games[1].Update(updateTime, { 0, 0 });

		const auto lobbyInfoPacket = networkClientManagers[1].LastPacketSent->As<MyPackets::LobbyInformationPacket>();

		ASSERT_EQ(lobbyInfoPacket->IsHost, p2Lobby.IsHost);
		ASSERT_EQ(lobbyInfoPacket->WaitingForOpponent, p2Lobby.WaitingForOpponent);
		ASSERT_EQ(lobbyInfoPacket->Player1Name.AsString(), p2Lobby.Player1.Name.AsString());
		ASSERT_EQ(lobbyInfoPacket->Player1Icon, p2Lobby.Player1.IconIndex);
		ASSERT_EQ(lobbyInfoPacket->Player2Name.AsString(), p2Lobby.Player2.Name.AsString());
		ASSERT_EQ(lobbyInfoPacket->Player2Icon, p2Lobby.Player2.IconIndex);
		ASSERT_EQ(lobbyInfoPacket->ChosenDeckType, p2Lobby.DeckType);
	}
}