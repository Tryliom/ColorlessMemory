#include "gui/guis/LobbyGui.h"

#include "Game.h"
#include "Logger.h"
#include "AssetManager.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/LeaveLobbyPacket.h"
#include "MyPackets/ChangeDeckPacket.h"

std::string ToString(DeckType deckType)
{
	switch (deckType)
	{
	case DeckType::Deck3x2:
		return "3x2";
	case DeckType::Deck7x2:
		return "7x2";
	case DeckType::Deck6x5:
		return "6x5";
	case DeckType::Deck7x6:
		return "7x6";
	case DeckType::Deck10x5:
		return "10x5";
	}

	return "Unknown";
}

LobbyGui::LobbyGui(Game& game, GameManager& gameManager, float width, float height) :
		_game(game),
		_gameManager(gameManager),
		_height(height),
		_width(width)
{
	// Create buttons
	auto launchButton = Button(
			sf::Vector2f(_width / 2.f, _height * 3.f / 4.f),
			sf::Vector2f(300, 50),
			true
	);

	launchButton.SetText({
			TextLine({ CustomText{ .Text = "START GAME", .Style = sf::Text::Style::Bold, .Size = 30 }})
	});
	launchButton.SetOnClick([this]()
	{
		_game.SendPacket(new MyPackets::StartGamePacket());
		_buttons[0].SetText({
				TextLine({ CustomText{ .Text = "STARTING...", .Style = sf::Text::Style::Bold, .Size = 30 }})
		});
		_buttons[0].SetOnClick([]()
		{});
	});
	launchButton.Disable();

	_buttons.emplace_back(launchButton);

	auto leaveButton = Button(
			sf::Vector2f(_width / 2.f, _height - 100.f),
			sf::Vector2f(200, 50),
			true
	);

	leaveButton.SetText({
			TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 24 }})
	});
	leaveButton.SetOnClick([this]()
	{
		_game.SendPacket(new MyPackets::LeaveLobbyPacket());
		_game.SetState(GameState::MAIN_MENU);
	});

	_buttons.emplace_back(leaveButton);

	// Create texts
	auto title = Text(
			sf::Vector2f(_width / 2.f, 100.f),
			{ TextLine({ CustomText{ .Text = "Lobby", .Size = 50 }}) }
	);
	auto waiting = Text(
			sf::Vector2f(_width / 2.f, _height / 2.f),
			{ TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }}) }
	);

	_texts.emplace_back(title);
	_texts.emplace_back(waiting);

	// Create player icons
	const auto& player1Position = sf::Vector2f(100, _height / 2.f - 200.f);
	const auto& player2Position = sf::Vector2f(_width - 100, _height / 2.f - 200.f);

	_player1 = PlayerUi(true, player1Position, false);
	_player2 = PlayerUi(false, player2Position, false);

	// Create and place deck icons
	const auto& deckIconSize = sf::Vector2f(210, 200);
	const auto& decksWidth = _deckIcons.size() * deckIconSize.x;
	const auto& startPosition = sf::Vector2f(_width / 2.f - decksWidth / 2.f, _height / 2.f + 100.f);

	for (std::size_t i = 0; i < _deckIcons.size(); i++)
	{
		const auto& position = startPosition + sf::Vector2f(i * deckIconSize.x, 0);
		const auto deckType = static_cast<DeckType>(i);

		_deckIcons[i].SetTexture(static_cast<DeckType>(i));
		_deckIcons[i].SetPosition(position + sf::Vector2f(deckIconSize.x / 2.f, -50));

		auto button = Button(
				position + deckIconSize / 2.f,
				sf::Vector2f(180, 50),
				true
		);

		button.SetText({
				TextLine({ CustomText{ .Text = "Deck " + ToString(deckType), .Size = 20 }})
		});

		button.SetOnClick([deckType, this]()
		{
			_gameManager.ChangeDeck(deckType);

			for (auto buttonIndex = 2; buttonIndex < _buttons.size(); ++buttonIndex)
			{
				_buttons[buttonIndex].Toggle(buttonIndex - 2 == static_cast<int>(deckType));
			}

			_game.SendPacket(new MyPackets::ChangeDeckPacket(deckType));
		});

		if (deckType == _gameManager.GetLobby().DeckType)
		{
			button.Toggle(true);
		}

		button.Disable(); // Disable all buttons by default

		_buttons.emplace_back(button);
	}
}

void LobbyGui::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		_game.SetState(GameState::GAME);
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::LobbyInformation))
	{
		const auto& lobby = _gameManager.GetLobby();

		if (lobby.WaitingForOpponent)
		{
			_buttons[0].Disable();
			_texts[1] = Text(
					sf::Vector2f(_width / 2.f, _height / 2.f),
					{ TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }}) }
			);
		}
		else
		{
			if (lobby.IsHost)
			{
				// Enable deck buttons
				for (std::size_t i = 0; i < _deckIcons.size(); ++i)
				{
					_buttons[i + 2].Enable();
				}

				_buttons[0].Enable();
			}
			else
			{
				// Disable deck buttons
				for (std::size_t i = 0; i < _deckIcons.size(); ++i)
				{
					_buttons[i + 2].Disable();
				}

				_buttons[0].Disable();
			}
			_texts[1] = Text(
					sf::Vector2f(_width / 2.f, _height / 2.f),
					{ TextLine({ CustomText{ .Text = "Ready to start !", .Size = 30 }}) }
			);
		}

		_player1.SetIcon({ static_cast<char>(lobby.Player1.IconIndex) });
		_player1.SetName(lobby.Player1.Name.AsString());
		_player2.SetIcon({ static_cast<char>(lobby.Player2.IconIndex) });
		_player2.SetName(lobby.Player2.Name.AsString());
	}
}

void LobbyGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const auto& lobby = _gameManager.GetLobby();

	if (!lobby.Player1.Name.AsString().empty())
	{
		target.draw(_player1, states);
	}

	if (!lobby.Player2.Name.AsString().empty())
	{
		target.draw(_player2, states);
	}

	if (lobby.IsHost && !lobby.WaitingForOpponent)
	{
		for (const auto& deckIcon: _deckIcons)
		{
			deckIcon.Draw(target, states);
		}
	}
}
