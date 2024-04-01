#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "GameManager.h"
#include "ClientNetworkInterface.h"
#include "ServerNetworkInterface.h"
#include "Logger.h"
#include "MyPackets/ChangeDeckPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "Random.h"
#include "MyPackets/CardInformationPacket.h"
#include "MyPackets/TurnPacket.h"

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
	std::queue<PacketData> PacketsSent;
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
		PacketsSent.push({ packet, clientId });
	}

	PacketData PopSentPacket()
	{
		if (PacketsSent.empty()) return {};

		auto packet = PacketsSent.front();
		PacketsSent.pop();
		return packet;
	}

	ClientId PopDisconnectedClient() override
	{
		if (DisconnectedClients.empty()) return {};

		auto client = DisconnectedClients.front();
		DisconnectedClients.pop();
		return client;
	}
};

void CompareLobbyInformations(MyPackets::LobbyInformationPacket* lobbyInformationPacket, GameData::Lobby lobby)
{
	ASSERT_EQ(lobbyInformationPacket->IsHost, lobby.IsHost);
	ASSERT_EQ(lobbyInformationPacket->WaitingForOpponent, lobby.WaitingForOpponent);
	ASSERT_EQ(lobbyInformationPacket->Player1Name.AsString(), lobby.Player1.Name.AsString());
	ASSERT_EQ(lobbyInformationPacket->Player1Icon, lobby.Player1.IconIndex);
	ASSERT_EQ(lobbyInformationPacket->Player2Name.AsString(), lobby.Player2.Name.AsString());
	ASSERT_EQ(lobbyInformationPacket->Player2Icon, lobby.Player2.IconIndex);
	ASSERT_EQ(lobbyInformationPacket->ChosenDeckType, lobby.DeckType);
}

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
	const auto cardRevealTime = sf::milliseconds(500);

	gameManagers[0].SetUsername(p1Name.AsString());
	gameManagers[0].SetPlayerIcon(p1Icon);
	gameManagers[1].SetUsername(p2Name.AsString());
	gameManagers[1].SetPlayerIcon(p2Icon);

	// 1. Each player join the lobby
	// 1.1 Player 1 joins the lobby
	{
		games[0].SetState(GameState::LOBBY);

		ASSERT_EQ(networkClientManagers[0].LastPacketSent->Type, static_cast<char>(MyPackets::MyPacketType::JoinLobby));

		serverNetwork.PacketsToProcess.push({ networkClientManagers[0].LastPacketSent, clientIds[0] });

		server.Update();

		auto packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::LobbyInformation));

		networkClientManagers[0].PacketsToProcess.push(packet.Packet->Clone());

		games[0].Update(updateTime, { 0, 0 });

		CompareLobbyInformations(packet.Packet->As<MyPackets::LobbyInformationPacket>(), p1Lobby);
		delete packet.Packet;
	}

	// 1.2 Player 2 joins the lobby
	{
		games[1].SetState(GameState::LOBBY);

		ASSERT_EQ(networkClientManagers[1].LastPacketSent->Type, static_cast<char>(MyPackets::MyPacketType::JoinLobby));

		serverNetwork.PacketsToProcess.push({ networkClientManagers[1].LastPacketSent, clientIds[1] });

		server.Update();

		// Packet is first sent to player 1
		auto packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::LobbyInformation));

		networkClientManagers[0].PacketsToProcess.push(packet.Packet->Clone());

		games[0].Update(updateTime, { 0, 0 });

		CompareLobbyInformations(packet.Packet->As<MyPackets::LobbyInformationPacket>(), p1Lobby);
		delete packet.Packet;

		// Packet is then sent to player 2
		packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::LobbyInformation));

		networkClientManagers[1].PacketsToProcess.push(packet.Packet->Clone());

		games[1].Update(updateTime, { 0, 0 });

		CompareLobbyInformations(packet.Packet->As<MyPackets::LobbyInformationPacket>(), p2Lobby);
		delete packet.Packet;
	}

	// 2. Choose a deck and start
	// 2.1 With player 2 -> check that he cannot start -> server doesn't send a packet
	{
		const DeckType deckType = DeckType::Deck3x2;

		serverNetwork.PacketsToProcess.push({ new MyPackets::ChangeDeckPacket(deckType), clientIds[1] });
		serverNetwork.PacketsToProcess.push({ new MyPackets::StartGamePacket(), clientIds[1] });

		server.Update();

		auto packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet, nullptr);
	}

	const DeckType deckType = DeckType::Deck6x5;
	const auto deckSize = 6 * 5;

	// 2.2 With player 1 -> check that he can start
	{
		serverNetwork.PacketsToProcess.push({ new MyPackets::ChangeDeckPacket(deckType), clientIds[0] });
		serverNetwork.PacketsToProcess.push({ new MyPackets::StartGamePacket(), clientIds[0] });

		server.Update();

		auto packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::StartGame));

		networkClientManagers[0].PacketsToProcess.push(packet.Packet->Clone());

		games[0].Update(updateTime, { 0, 0 });

		ASSERT_EQ(p1Game.YourTurn, packet.Packet->As<MyPackets::StartGamePacket>()->YourTurn);
		delete packet.Packet;

		packet = serverNetwork.PopSentPacket();

		ASSERT_EQ(packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::StartGame));

		networkClientManagers[1].PacketsToProcess.push(packet.Packet->Clone());

		games[1].Update(updateTime, { 0, 0 });

		ASSERT_EQ(p2Game.YourTurn, packet.Packet->As<MyPackets::StartGamePacket>()->YourTurn);
		ASSERT_EQ(p2Game.DeckType, packet.Packet->As<MyPackets::StartGamePacket>()->ChosenDeckType);
		delete packet.Packet;
	}

	// 3. Play the game until the end
	{
		int turn = 0;
		std::vector<CardIconIndex> deck;
		std::vector<CardIconIndex> foundCardIcons;

		for (int i = 0; i < deckSize / 2; i++)
		{
			deck.push_back(UNKNOWN_ICON_INDEX);
			deck.push_back(UNKNOWN_ICON_INDEX);
		}

		// Play turns until the end
		while (foundCardIcons.size() < deck.size() / 2)
		{
			LOG("Turn: " << turn++ << " - Found: " << foundCardIcons.size() << " / " << deck.size() / 2);

			auto& currentGame = p1Game.YourTurn ? games[0] : games[1];
			auto& otherGame = p1Game.YourTurn ? games[1] : games[0];
			auto& currentPlayerNetwork = p1Game.YourTurn ? networkClientManagers[0] : networkClientManagers[1];
			auto& otherPlayerNetwork = p1Game.YourTurn ? networkClientManagers[1] : networkClientManagers[0];
			auto& currentGameManager = p1Game.YourTurn ? gameManagers[0] : gameManagers[1];
			const auto& currentClientId = p1Game.YourTurn ? clientIds[0] : clientIds[1];

			const auto pairLeft = deck.size() / 2 - foundCardIcons.size();

			// Search for 2 random cards
			std::vector<CardIndex> leftCards;

			for (int i = 0; i < deck.size(); i++)
			{
				if (foundCardIcons.end() == std::find(foundCardIcons.begin(), foundCardIcons.end(), deck[i]))
				{
					leftCards.push_back(CardIndex(i));
				}
			}

			const auto card1 = leftCards[0];
			const auto card2 = leftCards[Random::Range(1, leftCards.size() - 1)];

			// Pick the cards
			{
				// Check if the player can pick the cards
				ASSERT_EQ(currentGameManager.ChooseACard(card1), true);
				ASSERT_EQ(currentGameManager.ChooseACard(card2), true);

				// Send card information packet to the server and check that each player receive the card icon info
				{
					serverNetwork.PacketsToProcess.push({ new MyPackets::CardInformationPacket(card1, UNKNOWN_ICON_INDEX), currentClientId });
					serverNetwork.PacketsToProcess.push({ new MyPackets::CardInformationPacket(card2, UNKNOWN_ICON_INDEX), currentClientId });

					server.Update();

					auto p1Card1Packet = serverNetwork.PopSentPacket();
					auto p2Card1Packet = serverNetwork.PopSentPacket();
					auto p1Card2Packet = serverNetwork.PopSentPacket();
					auto p2Card2Packet = serverNetwork.PopSentPacket();

					ASSERT_EQ(p1Card1Packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::CardInformation));
					ASSERT_EQ(p2Card1Packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::CardInformation));
					ASSERT_EQ(p1Card2Packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::CardInformation));
					ASSERT_EQ(p2Card2Packet.Packet->Type, static_cast<char>(MyPackets::MyPacketType::CardInformation));

					currentPlayerNetwork.PacketsToProcess.push(p1Card1Packet.Packet->Clone());
					currentPlayerNetwork.PacketsToProcess.push(p1Card2Packet.Packet->Clone());
					otherPlayerNetwork.PacketsToProcess.push(p2Card1Packet.Packet->Clone());
					otherPlayerNetwork.PacketsToProcess.push(p2Card2Packet.Packet->Clone());

					// Double update for the animations
					currentGame.Update(updateTime, { 0, 0 });
					currentGame.Update(cardRevealTime, { 0, 0 });
					otherGame.Update(updateTime, { 0, 0 });
					otherGame.Update(cardRevealTime, { 0, 0 });

					const auto& p1Card1InformationPacket = p1Card1Packet.Packet->As<MyPackets::CardInformationPacket>();
					const auto& p2Card1InformationPacket = p2Card1Packet.Packet->As<MyPackets::CardInformationPacket>();
					const auto& p1Card2InformationPacket = p1Card2Packet.Packet->As<MyPackets::CardInformationPacket>();
					const auto& p2Card2InformationPacket = p2Card2Packet.Packet->As<MyPackets::CardInformationPacket>();

					ASSERT_EQ(p1Card1InformationPacket->CardIndexInDeck, card1);
					ASSERT_EQ(p2Card1InformationPacket->CardIndexInDeck, card1);
					ASSERT_EQ(p1Card2InformationPacket->CardIndexInDeck, card2);
					ASSERT_EQ(p2Card2InformationPacket->CardIndexInDeck, card2);

					ASSERT_NE(p1Card1InformationPacket->IconIndex, UNKNOWN_ICON_INDEX);
					ASSERT_NE(p2Card1InformationPacket->IconIndex, UNKNOWN_ICON_INDEX);
					ASSERT_NE(p1Card2InformationPacket->IconIndex, UNKNOWN_ICON_INDEX);
					ASSERT_NE(p2Card2InformationPacket->IconIndex, UNKNOWN_ICON_INDEX);


					ASSERT_EQ(p1Game.CardIndex1, card1);
					ASSERT_EQ(p2Game.CardIndex1, card1);
					ASSERT_EQ(p1Game.CardIndex2, card2);
					ASSERT_EQ(p2Game.CardIndex2, card2);

					deck[card1.Index] = p1Card1InformationPacket->IconIndex;
					deck[card2.Index] = p1Card2InformationPacket->IconIndex;

					if (deck[card1.Index] == deck[card2.Index])
					{
						foundCardIcons.push_back(deck[card1.Index]);
					}

					delete p1Card1Packet.Packet;
					delete p2Card1Packet.Packet;
					delete p1Card2Packet.Packet;
					delete p2Card2Packet.Packet;
				}
			}

			// Check turn packet
			if (pairLeft > 1)
			{
				const auto p1Turn = p1Game.YourTurn;
				const auto p2Turn = p2Game.YourTurn;

				auto p1TurnPacket = serverNetwork.PopSentPacket();
				auto p2TurnPacket = serverNetwork.PopSentPacket();

				ASSERT_EQ(p1TurnPacket.Packet->Type, static_cast<char>(MyPackets::MyPacketType::Turn));
				ASSERT_EQ(p2TurnPacket.Packet->Type, static_cast<char>(MyPackets::MyPacketType::Turn));

				networkClientManagers[0].PacketsToProcess.push(p1TurnPacket.Packet->Clone());
				networkClientManagers[1].PacketsToProcess.push(p2TurnPacket.Packet->Clone());

				games[0].Update(updateTime, { 0, 0 });
				games[0].Update(updateTime, { 0, 0 });
				games[1].Update(updateTime, { 0, 0 });
				games[1].Update(updateTime, { 0, 0 });

				const auto& p1TurnPacketData = p1TurnPacket.Packet->As<MyPackets::TurnPacket>();
				const auto& p2TurnPacketData = p2TurnPacket.Packet->As<MyPackets::TurnPacket>();

				ASSERT_EQ(p1Game.YourTurn, p1TurnPacketData->YourTurn);
				ASSERT_EQ(p2Game.YourTurn, p2TurnPacketData->YourTurn);

				if (deck[card1.Index] == deck[card2.Index])
				{
					ASSERT_EQ(p1Game.YourTurn, p1Turn);
					ASSERT_EQ(p2Game.YourTurn, p2Turn);

					LOG("Found a pair");
				}
				else
				{
					ASSERT_NE(p1Game.YourTurn, p1Turn);
					ASSERT_NE(p2Game.YourTurn, p2Turn);
				}

				delete p1TurnPacket.Packet;
				delete p2TurnPacket.Packet;
			}
		}
	}
}

//TODO: Test lobbies