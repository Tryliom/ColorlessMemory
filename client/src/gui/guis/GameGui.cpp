#include "gui/guis/GameGui.h"

#include "Game.h"
#include "AssetManager.h"
#include "Constants.h"

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

	// Max size for cards
	const auto& maxWidth = Game::WIDTH - 600.f;
	const auto& maxHeight = Game::HEIGHT - 200.f;

	auto cardSize = sf::Vector2f(AssetManager::GetCardIcon(0).getSize());
	auto xOffset = 20.f;
	auto yOffset = 20.f;
	auto width = cardSize.x * _xCards + xOffset * (_xCards - 1);
	auto height = cardSize.y * _yCards + yOffset * (_yCards - 1);

	// Calculate scale to fit the cards in the screen
	auto scale = std::min(maxWidth / width, maxHeight / height);

	if (scale > 1.f) scale = 1.f;

	cardSize *= scale;
	xOffset *= scale;
	yOffset *= scale;
	width = cardSize.x * _xCards + xOffset * (_xCards - 1);
	height = cardSize.y * _yCards + yOffset * (_yCards - 1);

	auto startX = (Game::WIDTH - width) / 2.f;
	auto startY = (Game::HEIGHT - height) / 2.f;

	if (scale < 1.f)
	{
		startX -= xOffset;
	}

	for (std::size_t i = 0; i < _xCards * _yCards; ++i)
	{
		const auto& x = i % _xCards;
		const auto& y = i / _xCards;

		_playCards.emplace_back(gameData.DeckType, -1);
		_playCards.back().SetPosition({ startX + x * (cardSize.x + xOffset), startY + y * (cardSize.y + yOffset) });
		_playCards.back().SetOnClicked([this, i] { OnSelectPlayCard(i); });
		_playCards.back().SetScale(scale);
	}

	// Create texts
	auto title = Text(
			sf::Vector2f(Game::WIDTH / 2.f, 100.f),
			{
					TextLine({ CustomText{ .Text = "Game", .Size = 50 }})
			}
	);

	_texts.emplace_back(title);

	// Create player icons
	const auto& lobby = Game::GetLobby();
	const auto& player1Position = sf::Vector2f(50, Game::HEIGHT / 2.f - 200.f);
	const auto& player2Position = sf::Vector2f(Game::WIDTH - 50, Game::HEIGHT / 2.f - 200.f);

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

	// Set turn
	_yourTurn = gameData.YourTurn;

	// Create turn text
	auto turn = Text(
			sf::Vector2f(Game::WIDTH / 2.f, 150.f),
			{
					TextLine({ CustomText{ .Text = _yourTurn ? "Your turn" : "Opponent's turn", .Size = 30 }})
			}
	);

	_texts.emplace_back(turn);

	auto leaveButton = Button(
			sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT - 100.f),
			sf::Vector2f(200.f, 50.f),
			true
	);

	leaveButton.SetText({
			TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 30 }})
	});
	leaveButton.SetOnClick([]()
	{
		Game::SendPacket(new LeaveGamePacket());
		Game::SetState(GameState::MAIN_MENU);
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

void GameGui::OnUpdate(sf::Time elapsed)
{
	const auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(Game::GetWindow()));
	auto& gameData = Game::GetGame();

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

	if (_beforeScoringTimer > 0.f)
	{
		_beforeScoringTimer -= elapsed.asSeconds();

		if (_beforeScoringTimer <= 0.f)
		{
			if (_playCards[gameData.CardIndex1].GetIconIndex() == _playCards[gameData.CardIndex2].GetIconIndex())
			{
				_playCards[gameData.CardIndex1].Disable();
				_playCards[gameData.CardIndex2].Disable();

				auto& player = _yourTurn && gameData.IsFirstPlayer || !_yourTurn && !gameData.IsFirstPlayer ? _player1 : _player2;
				auto& score = _yourTurn && gameData.IsFirstPlayer || !_yourTurn && !gameData.IsFirstPlayer ? gameData.Player1Score : gameData.Player2Score;

				score++;
				player.SetScore(static_cast<int>(score));

				_endTurnTimer = 0.25f;
			}
			else
			{
				_playCards[gameData.CardIndex1].StartFlip();
				_playCards[gameData.CardIndex2].StartFlip();

				_endTurnTimer = 0.5f;
			}
		}
	}

	if (_endTurnTimer > 0.f)
	{
		_endTurnTimer -= elapsed.asSeconds();

		if (_endTurnTimer <= 0.f)
		{
			if (gameData.Player1Score + gameData.Player2Score == _playCards.size() / 2)
			{
				const auto& player1Win = gameData.Player1Score > gameData.Player2Score;
				const auto& isPlayer1 = gameData.IsFirstPlayer;

				_gameOver = true;

				_texts[1] = Text(
						sf::Vector2f(Game::WIDTH / 2.f, 150.f),
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
}

void GameGui::OnPacketReceived(const Packet& packet)
{
	if (packet.type == PacketType::CardInformation)
	{
		auto& cardInformationPacket = dynamic_cast<const CardInformationPacket&>(packet);

		_playCards[cardInformationPacket.CardIndex].SetIndex(cardInformationPacket.IconIndex);

		SelectCard(cardInformationPacket.CardIndex);
	}
	else if (packet.type == PacketType::LeaveGame)
	{
		_gameOver = true;

		_texts[1] = Text(
				sf::Vector2f(Game::WIDTH / 2.f, 150.f),
				{
						TextLine({ CustomText{ .Text = "Other player leave", .Size = 30 }})
				}
		);
	}
}

void GameGui::OnSelectPlayCard(std::size_t i)
{
	auto& gameData = Game::GetGame();

	if (!_yourTurn || gameData.CardIndex1 != -1 && gameData.CardIndex2 != -1) return;
	if (gameData.CardIndex1 == i || gameData.CardIndex2 == i) return;

	auto& card = _playCards[i];

	if (card.IsFlipping()) return;

	if (gameData.CardIndex1 == -1)
	{
		gameData.CardIndex1 = i;
	}
	else if (gameData.CardIndex2 == -1)
	{
		gameData.CardIndex2 = i;
	}

	// Send packet to know icon and other player
	Game::SendPacket(new CardInformationPacket(i, UNKNOWN_ICON_INDEX));
}

void GameGui::SelectCard(std::size_t i)
{
	auto& gameData = Game::GetGame();
	auto& card = _playCards[i];

	card.StartFlip();

	if (gameData.CardIndex1 == -1 || gameData.CardIndex1 == i)
	{
		gameData.CardIndex1 = i;
	}
	else if (gameData.CardIndex2 == -1 || gameData.CardIndex2 == i)
	{
		gameData.CardIndex2 = i;
	}

	if (gameData.CardIndex2 == i)
	{
		_blockInput = true;
		_beforeScoringTimer = 1.f;
	}
}

void GameGui::StartTurn()
{
	auto& gameData = Game::GetGame();

	_blockInput = false;
	gameData.CardIndex1 = -1;
	gameData.CardIndex2 = -1;
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
			sf::Vector2f(Game::WIDTH / 2.f, 150.f),
			{
					TextLine({ CustomText{ .Text = _yourTurn ? "Your turn" : "Opponent's turn", .Size = 30 }})
			}
	);
}