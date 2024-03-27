#include "PacketManager.h"
#include "MyPackets.h"
#include "MyPackets/LobbyInformationPacket.h"
#include "MyPackets/ChangeDeckPacket.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/LeaveLobbyPacket.h"
#include "MyPackets/TurnPacket.h"
#include "MyPackets/CardInformationPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/LeaveGamePacket.h"

#include <gtest/gtest.h>

struct LobbyInformationData
{
	PlayerName Player1Name{};
	PlayerName Player2Name{};
	IconType Player1Icon{};
	IconType Player2Icon{};
	DeckType ChosenDeckType{};
	bool IsHost{};
	bool WaitingForOpponent{};
};

struct TestLobbyInformation : public ::testing::TestWithParam<LobbyInformationData> {};

INSTANTIATE_TEST_SUITE_P(LobbyInformation, TestLobbyInformation, testing::Values(
	LobbyInformationData{ PlayerName("Player1"), PlayerName("Player2"), IconType::Icon1, IconType::Icon2, DeckType::Deck3x2, true, true },
	LobbyInformationData{ PlayerName("93  f43hs1"), PlayerName("Tutu top"), IconType::Icon3, IconType::Icon4, DeckType::Deck6x5, false, false },
	LobbyInformationData{ PlayerName("Player1"), PlayerName("Player2"), IconType::Icon5, IconType::Icon1, DeckType::Deck7x2, true, false },
	LobbyInformationData{ PlayerName("Player1"), PlayerName("Player2"), IconType::Icon2, IconType::Icon3, DeckType::Deck7x6, false, true }
));

TEST_P(TestLobbyInformation, LobbyInformation)
{
	MyPackets::RegisterMyPackets();

	const auto [player1Name, player2Name, player1Icon, player2Icon, chosenDeckType, isHost, waitingForOpponent] = GetParam();
	MyPackets::LobbyInformationPacket packet(isHost, waitingForOpponent, player1Name, player2Name, player1Icon, player2Icon, chosenDeckType);
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto lobbyInformationPacket = *newPacket->As<MyPackets::LobbyInformationPacket>();

	EXPECT_EQ(lobbyInformationPacket.IsHost, isHost);
	EXPECT_EQ(lobbyInformationPacket.WaitingForOpponent, waitingForOpponent);
	EXPECT_EQ(lobbyInformationPacket.Player1Name, player1Name);
	EXPECT_EQ(lobbyInformationPacket.Player2Name, player2Name);
	EXPECT_EQ(lobbyInformationPacket.Player1Icon, player1Icon);
	EXPECT_EQ(lobbyInformationPacket.Player2Icon, player2Icon);
	EXPECT_EQ(lobbyInformationPacket.ChosenDeckType, chosenDeckType);
}

struct ChangeDeckData
{
	DeckType ChosenDeckType{};
};

struct TestChangeDeck : public ::testing::TestWithParam<ChangeDeckData> {};

INSTANTIATE_TEST_SUITE_P(ChangeDeck, TestChangeDeck, testing::Values(
	ChangeDeckData{ DeckType::Deck3x2 },
	ChangeDeckData{ DeckType::Deck6x5 },
	ChangeDeckData{ DeckType::Deck7x2 },
	ChangeDeckData{ DeckType::Deck7x6 },
	ChangeDeckData{ DeckType::Deck10x5 }
));

TEST_P(TestChangeDeck, ChangeDeck)
{
	MyPackets::RegisterMyPackets();

	const auto [chosenDeckType] = GetParam();
	MyPackets::ChangeDeckPacket packet(chosenDeckType);
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto changeDeckPacket = *newPacket->As<MyPackets::ChangeDeckPacket>();

	EXPECT_EQ(changeDeckPacket.ChosenDeckType, chosenDeckType);
}

struct JoinLobbyData
{
	PlayerName Name{};
	IconType IconIndex{};
};

struct TestJoinLobby : public ::testing::TestWithParam<JoinLobbyData> {};

INSTANTIATE_TEST_SUITE_P(JoinLobby, TestJoinLobby, testing::Values(
	JoinLobbyData{ PlayerName("Player1"), IconType::Icon1 },
	JoinLobbyData{ PlayerName("Player2"), IconType::Icon2 },
	JoinLobbyData{ PlayerName("Player3"), IconType::Icon3 },
	JoinLobbyData{ PlayerName("Player4"), IconType::Icon4 },
	JoinLobbyData{ PlayerName("Player5"), IconType::Icon5 }
));

TEST_P(TestJoinLobby, JoinLobby)
{
	MyPackets::RegisterMyPackets();

	const auto [name, iconIndex] = GetParam();
	MyPackets::JoinLobbyPacket packet(name.AsString(), iconIndex);
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto joinLobbyPacket = *newPacket->As<MyPackets::JoinLobbyPacket>();

	EXPECT_EQ(joinLobbyPacket.Name, name);
	EXPECT_EQ(joinLobbyPacket.IconIndex, iconIndex);
}

TEST(TestLeaveLobby, LeaveLobby)
{
	MyPackets::RegisterMyPackets();

	MyPackets::LeaveLobbyPacket packet;
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto leaveLobbyPacket = *newPacket->As<MyPackets::LeaveLobbyPacket>();

	EXPECT_EQ(leaveLobbyPacket.Type, static_cast<char>(MyPackets::MyPacketType::LeaveLobby));
}

struct TurnData
{
	bool YourTurn{};
};

struct TestTurn : public ::testing::TestWithParam<TurnData> {};

INSTANTIATE_TEST_SUITE_P(Turn, TestTurn, testing::Values(
	TurnData{ true },
	TurnData{ false }
));

TEST_P(TestTurn, Turn)
{
	MyPackets::RegisterMyPackets();

	const auto [yourTurn] = GetParam();
	MyPackets::TurnPacket packet(yourTurn);
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto turnPacket = *newPacket->As<MyPackets::TurnPacket>();

	EXPECT_EQ(turnPacket.YourTurn, yourTurn);
}

struct CardInformationData
{
	CardIndex CardIndexInDeck{};
	CardIconIndex IconIndex{};
};

struct TestCardInformation : public ::testing::TestWithParam<CardInformationData> {};

INSTANTIATE_TEST_SUITE_P(CardInformation, TestCardInformation, testing::Values(
	CardInformationData{ CardIndex(0), CardIconIndex(0) },
	CardInformationData{ CardIndex(1), CardIconIndex(1) },
	CardInformationData{ CardIndex(2), CardIconIndex(2) },
	CardInformationData{ CardIndex(3), CardIconIndex(3) },
	CardInformationData{ CardIndex(4), CardIconIndex(4) }
));

TEST_P(TestCardInformation, CardInformation)
{
	MyPackets::RegisterMyPackets();

	const auto [cardIndexInDeck, iconIndex] = GetParam();
	MyPackets::CardInformationPacket packet(cardIndexInDeck, iconIndex);
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto cardInformationPacket = *newPacket->As<MyPackets::CardInformationPacket>();

	EXPECT_EQ(cardInformationPacket.CardIndexInDeck, cardIndexInDeck);
	EXPECT_EQ(cardInformationPacket.IconIndex, iconIndex);
}

TEST(TestStartGame, StartGame)
{
	MyPackets::RegisterMyPackets();

	MyPackets::StartGamePacket packet;
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto startGamePacket = *newPacket->As<MyPackets::StartGamePacket>();

	EXPECT_EQ(startGamePacket.Type, static_cast<char>(MyPackets::MyPacketType::StartGame));
}

TEST(TestLeaveGame, LeaveGame)
{
	MyPackets::RegisterMyPackets();

	MyPackets::LeaveGamePacket packet;
	sf::Packet* sfPacket = PacketManager::ToSfPacket(&packet);
	Packet* newPacket = PacketManager::FromPacket(sfPacket);
	auto leaveGamePacket = *newPacket->As<MyPackets::LeaveGamePacket>();

	EXPECT_EQ(leaveGamePacket.Type, static_cast<char>(MyPackets::MyPacketType::LeaveGame));
}