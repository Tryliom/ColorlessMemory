#include "gui/guis/GameGui.h"

#include "Game.h"
#include "AssetManager.h"

GameGui::GameGui()
{
	// Setup cards
	auto& gameData = Game::GetGame();

	switch (gameData.DeckType)
	{
	case DeckType::Deck3x2:
		_xCards = 3;
		_yCards = 2;
		break;
	case DeckType::Deck7x2:
		_xCards = 7;
		_yCards = 2;
		break;
	case DeckType::Deck6x5:
		_xCards = 6;
		_yCards = 5;
		break;
	case DeckType::Deck7x6:
		_xCards = 7;
		_yCards = 6;
		break;
	case DeckType::Deck10x5:
		_xCards = 10;
		_yCards = 5;
		break;
	}

	// Create play cards
	const auto& cardSize = sf::Vector2f(AssetManager::GetCardIcon(0).getSize());
	const auto& xOffset = 20.f;
	const auto& yOffset = 20.f;
	const auto& width = cardSize.x * _xCards + xOffset * (_xCards - 1);
	const auto& height = cardSize.y * _yCards + yOffset * (_yCards - 1);
	const auto& startX = (Game::WIDTH - width) / 2.f;
	const auto& startY = (Game::HEIGHT - height) / 2.f;

	//TODO: Calculate scale to fit the cards in the screen

	for (std::size_t i = 0; i < _xCards * _yCards; ++i)
	{
		const auto& x = i % _xCards;
		const auto& y = i / _xCards;

		_playCards.emplace_back(gameData.DeckType, -1);
		_playCards.back().SetPosition({ startX + x * (cardSize.x + xOffset),startY + y * (cardSize.y + yOffset) });
		//TODO: Add on click func that redirect on a class func with info about the card
	}

	// Create texts
	auto title = Text(
		sf::Vector2f(Game::WIDTH / 2.f, 100.f),
		{
			TextLine({CustomText{.Text = "Game", .Size = 50}})
		}
	);

	_texts.emplace_back(title);

	// Create player icons
	const auto& lobby = Game::GetLobby();
	const auto& player1Position = sf::Vector2f(100, Game::HEIGHT / 2.f - 200.f);
	const auto& player2Position = sf::Vector2f(Game::WIDTH - 100, Game::HEIGHT / 2.f - 200.f);

	_player1 = PlayerUi(true, player1Position, true);
	_player2 = PlayerUi(false, player2Position, true);

	// Set player names
	_player1.SetName(lobby.Player1.Name);
	_player2.SetName(lobby.Player2.Name);

	// Set player icons
	_player1.SetIcon(lobby.Player1.IconIndex);
	_player2.SetIcon(lobby.Player2.IconIndex);

	// Set player scores
	_player1.SetScore(0);
	_player2.SetScore(0);

	//TODO: Add leave button and implement leave game func for server and handle to redirect other user
}

void GameGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const auto& card : _playCards)
	{
		target.draw(card, states);
	}

	for (const auto& text : _texts)
	{
		target.draw(text, states);
	}

	target.draw(_player1, states);
	target.draw(_player2, states);
}

void GameGui::OnUpdate(sf::Time elapsed)
{
	const auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(Game::GetWindow()));

	for (auto& card : _playCards)
	{
		card.Update(elapsed);

		if (card.IsFlipping()) continue;

		if (card.GetGlobalBounds().contains(mousePosition))
		{
			if (!card.IsHover())
			{
				card.OnHover(true);
			}
		}
		else
		{
			if (card.IsHover())
			{
				card.OnHover(false);
			}
		}
	}
}

void GameGui::OnPacketReceived(const Packet& packet)
{
	if (packet.type == PacketType::CardInformation)
	{
		auto& cardInformationPacket = dynamic_cast<const CardInformationPacket&>(packet);

		_playCards[cardInformationPacket.CardIndex].SetIndex(cardInformationPacket.CardIndex);
	}
	else if (packet.type == PacketType::Turn)
	{
		auto& turnPacket = dynamic_cast<const TurnPacket&>(packet);

		Game::GetGame().YourTurn = turnPacket.YourTurn;

		//TODO: Diplay a text to show who's turn it is
	}
}
