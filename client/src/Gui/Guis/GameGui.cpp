#include "gui/guis/GameGui.h"

#include "Game.h"
#include "GameManager.h"
#include "AssetManager.h"
#include "Constants.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/CardInformationPacket.h"

GameGui::GameGui(Game& game, GameManager& gameManager, float width, float height) :
	_game(game), _gameManager(gameManager), _height(height), _width(width)
{
	// Setup cards
	const auto& gameData = _gameManager.GetGame();

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

	// Max size for cards
	const auto& maxWidth = _width - 600.f;
	const auto& maxHeight = _height - 200.f;

	auto cardSize = AssetManager::IsInitialized() ? sf::Vector2f(AssetManager::GetCardIcon(DEFAULT_ICON_INDEX).getSize()) : sf::Vector2f(20, 20);
	auto xOffset = 20.f;
	auto yOffset = 20.f;
	auto gameAreaWidth = cardSize.x * _xCards + xOffset * (_xCards - 1);
	auto gameAreaHeight = cardSize.y * _yCards + yOffset * (_yCards - 1);

	// Calculate scale to fit the cards in the screen
	auto scale = std::min(maxWidth / gameAreaWidth, maxHeight / gameAreaHeight);

	if (scale > 1.f) scale = 1.f;

	cardSize *= scale;
	xOffset *= scale;
	yOffset *= scale;
	gameAreaWidth = cardSize.x * _xCards + xOffset * (_xCards - 1);
	gameAreaHeight = cardSize.y * _yCards + yOffset * (_yCards - 1);

	auto startX = (_width - gameAreaWidth) / 2.f;
	auto startY = (_height - gameAreaHeight) / 2.f;

	if (scale < 1.f)
	{
		startX -= xOffset;
	}

	for (std::size_t i = 0; i < _xCards * _yCards; ++i)
	{
		const auto& x = i % _xCards;
		const auto& y = i / _xCards;

		_playCards.emplace_back(gameData.DeckType, UNKNOWN_ICON_INDEX);
		_playCards.back().SetPosition({ startX + x * (cardSize.x + xOffset), startY + y * (cardSize.y + yOffset) });
		_playCards.back().SetOnClicked([this, i] { OnSelectPlayCard(CardIndex {static_cast<char>(i)}); });
		_playCards.back().SetScale(scale);
	}

	// Create texts
	auto title = Text(
			sf::Vector2f(_width / 2.f, 100.f),
			{
					TextLine({ CustomText{ .Text = "Game", .Size = 50 }})
			}
	);

	_texts.emplace_back(title);

	// Create player icons
	const auto& lobby = _gameManager.GetLobby();
	const auto& player1Position = sf::Vector2f(50, _height / 2.f - 200.f);
	const auto& player2Position = sf::Vector2f(_width - 50, _height / 2.f - 200.f);

	_player1 = PlayerUi(true, player1Position, true);
	_player2 = PlayerUi(false, player2Position, true);

	// Set player names
	_player1.SetName(lobby.Player1.Name.AsString());
	_player2.SetName(lobby.Player2.Name.AsString());

	// Set player icons
	_player1.SetIcon({ static_cast<char>(lobby.Player1.IconIndex) });
	_player2.SetIcon({ static_cast<char>(lobby.Player2.IconIndex) });

	// Set player scores
	_player1.SetScore(0);
	_player2.SetScore(0);

	// Set turn
	_yourTurn = gameData.YourTurn;

	// Create turn text
	auto turn = Text(
			sf::Vector2f(_width / 2.f, 150.f),
			{
					TextLine({ CustomText{ .Text = _yourTurn ? "Your turn" : "Opponent's turn", .Size = 30 }})
			}
	);

	_texts.emplace_back(turn);

	auto leaveButton = Button(
			sf::Vector2f(_width / 2.f, _height - 100.f),
			sf::Vector2f(200.f, 50.f),
			true
	);

	leaveButton.SetText({
			TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 30 }})
	});
	leaveButton.SetOnClick([this]()
	{
		_game.SendPacket(new MyPackets::LeaveGamePacket());
		_game.SetState(GameState::MAIN_MENU);
	});

	_buttons.emplace_back(leaveButton);
}

void GameGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!_gameOver)
	{
		for (const auto& card: _playCards)
		{
			target.draw(card, states);
		}
	}

	for (const auto& text: _texts)
	{
		target.draw(text, states);
	}

	target.draw(_player1, states);
	target.draw(_player2, states);
}

void GameGui::OnCheckInputs(sf::Event event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			for (auto& card: _playCards)
			{
				if (card.IsFlipping()) continue;

				if (card.IsHover() && !_blockInput)
				{
					card.Click();
				}
			}
		}
	}
}

void GameGui::OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition)
{
	auto& gameData = _gameManager.GetGame();

	for (auto& card: _playCards)
	{
		card.Update(elapsed);

		if (card.IsFlipping()) continue;

		if (card.GetGlobalBounds().contains(mousePosition))
		{
			if (!card.IsHover() && gameData.YourTurn)
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

	UpdateScoringTimer(elapsed);
	UpdateEndTimer(elapsed);
}

void GameGui::UpdateScoringTimer(sf::Time elapsed)
{
	if (_beforeScoringTimer > 0.f)
	{
		_beforeScoringTimer -= elapsed.asSeconds();

		if (_beforeScoringTimer > 0.f) return;

		const auto& gameData = _gameManager.GetGame();
		const auto cardIndex1 = gameData.CardIndex1.Index;
		const auto cardIndex2 = gameData.CardIndex2.Index;

		if (_playCards[cardIndex1].GetIconIndex() == _playCards[cardIndex2].GetIconIndex())
		{
			_playCards[cardIndex1].Disable();
			_playCards[cardIndex2].Disable();

			auto& player = _yourTurn && gameData.IsFirstPlayer || !_yourTurn && !gameData.IsFirstPlayer ? _player1 : _player2;
			auto& score = _yourTurn && gameData.IsFirstPlayer || !_yourTurn && !gameData.IsFirstPlayer ? gameData.Player1Score : gameData.Player2Score;

			_gameManager.IncreaseScore(_yourTurn && gameData.IsFirstPlayer || !_yourTurn && !gameData.IsFirstPlayer ? 0 : 1);

			player.SetScore(static_cast<int>(score));

			_endTurnTimer = 0.25f;
		}
		else
		{
			_playCards[cardIndex1].StartFlip();
			_playCards[cardIndex2].StartFlip();

			_endTurnTimer = 0.5f;
		}
	}
}

void GameGui::UpdateEndTimer(sf::Time elapsed)
{
	if (_endTurnTimer > 0.f)
	{
		_endTurnTimer -= elapsed.asSeconds();

		if (_endTurnTimer > 0.f) return;

		const auto& gameData = _gameManager.GetGame();

		if (gameData.Player1Score + gameData.Player2Score == _playCards.size() / 2)
		{
			const auto& player1Win = gameData.Player1Score > gameData.Player2Score;
			const auto& isPlayer1 = gameData.IsFirstPlayer;

			_gameManager.EndTurn();
			_gameOver = true;

			_texts[1] = Text(
					sf::Vector2f(_width / 2.f, 150.f),
					{
							TextLine({ CustomText{ .Text = player1Win == isPlayer1 ? "You win" : "You lose", .Size = 30 }})
					}
			);
		}
		else
		{
			StartTurn();
		}
	}
}

void GameGui::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::CardInformation))
	{
		auto cardInformationPacket = *packet.As<MyPackets::CardInformationPacket>();

		_playCards[cardInformationPacket.CardIndexInDeck.Index].SetIndex(cardInformationPacket.IconIndex);

		SelectCard(cardInformationPacket.CardIndexInDeck);
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::LeaveGame))
	{
		_gameOver = true;

		_texts[1] = Text(
				sf::Vector2f(_width / 2.f, 150.f),
				{
						TextLine({ CustomText{ .Text = "Other player leave", .Size = 30 }})
				}
		);
	}
}

void GameGui::OnSelectPlayCard(CardIndex cardIndex)
{
	auto& card = _playCards[cardIndex.Index];

	if (card.IsFlipping()) return;

	if (!_gameManager.ChooseACard(cardIndex)) return;

	// Send packet to know icon and other player
	_game.SendPacket(new MyPackets::CardInformationPacket(cardIndex, UNKNOWN_ICON_INDEX));
}

void GameGui::SelectCard(CardIndex cardIndex)
{
	auto& gameData = _gameManager.GetGame();
	auto& card = _playCards[cardIndex.Index];

	card.StartFlip();

	if (gameData.CardIndex2 == cardIndex)
	{
		_blockInput = true;
		_beforeScoringTimer = 1.f;
	}
}

void GameGui::StartTurn()
{
	const auto& gameData = _gameManager.GetGame();

	_blockInput = false;
	_gameManager.EndTurn();
	_yourTurn = gameData.YourTurn;

	if (!_yourTurn)
	{
		for (auto& card: _playCards)
		{
			card.OnHover(false);
		}
	}

	if (_gameOver) return;

	_texts[1] = Text(
			sf::Vector2f(_width / 2.f, 150.f),
			{
					TextLine({ CustomText{ .Text = _yourTurn ? "Your turn" : "Opponent's turn", .Size = 30 }})
			}
	);
}