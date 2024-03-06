#include "gui/guis/LobbyGui.h"

#include "Game.h"
#include "Logger.h"
#include "Packet.h"
#include "AssetManager.h"

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

LobbyGui::LobbyGui()
{
	// Create buttons
	auto launchButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT * 3.f / 4.f),
		sf::Vector2f(300, 50),
		true
	);

	launchButton.SetText({
		TextLine({ CustomText{ .Text = "START GAME", .Style = sf::Text::Style::Bold, .Size = 30 }})
	});
	launchButton.SetOnClick([this]()
	{
	  Game::SendPacket(new StartGamePacket());
	  _buttons[0].SetText({
		  TextLine({ CustomText{ .Text = "STARTING...", .Style = sf::Text::Style::Bold, .Size = 30 }})
	  });
	  _buttons[0].SetOnClick([]()
	  {});
	});
	launchButton.Disable();

	_buttons.emplace_back(launchButton);

	auto leaveButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT - 100.f),
		sf::Vector2f(200, 50),
		true
	);

	leaveButton.SetText({
		TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 24 }})
	});
	leaveButton.SetOnClick([]()
	{
	  Game::SendPacket(new LeaveLobbyPacket());
	  Game::SetState(GameState::MAIN_MENU);
	});

	_buttons.emplace_back(leaveButton);

	// Create texts
	auto title = Text(
		sf::Vector2f(Game::WIDTH / 2.f, 100.f),
		{ TextLine({ CustomText{ .Text = "Lobby", .Size = 50 }}) }
	);
	auto waiting = Text(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
		{ TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }}) }
	);

	_texts.emplace_back(title);
	_texts.emplace_back(waiting);

	// Create player icons
	const auto& defaultIcon = &AssetManager::GetCardIcon(0);
	const auto& player1Icon = &AssetManager::GetTexture(TextureType::PLAYER1_ICON_BACKGROUND);
	const auto& player2Icon = &AssetManager::GetTexture(TextureType::PLAYER2_ICON_BACKGROUND);
	const auto& iconSize = defaultIcon->getSize();
	const auto& iconSizeF = sf::Vector2f(iconSize.x, iconSize.y);
	const auto& player1Position = sf::Vector2f(100, Game::HEIGHT / 2.f - 200.f);
	const auto& player2Position = sf::Vector2f(Game::WIDTH - 100, Game::HEIGHT / 2.f - 200.f);
	const auto& shadowOffset = sf::Vector2f(0, 5);
	const auto& shadowColor = sf::Color(0, 0, 0, 100);

	// Create player icons and names for each side
	_player1Icon = sf::RectangleShape(iconSizeF);
	_player1Icon.setTexture(defaultIcon);
	_player1Icon.setPosition(player1Position);
	_player1IconShadow = sf::RectangleShape(iconSizeF);
	_player1IconShadow.setTexture(defaultIcon);
	_player1IconShadow.setPosition(player1Position + shadowOffset * 0.5f);
	_player1IconShadow.setFillColor(shadowColor);
	_player1Background = sf::RectangleShape(iconSizeF);
	_player1Background.setTexture(player1Icon);
	_player1Background.setPosition(player1Position);
	_player1BackgroundShadow = sf::RectangleShape(iconSizeF);
	_player1BackgroundShadow.setTexture(player1Icon);
	_player1BackgroundShadow.setPosition(player1Position + shadowOffset);
	_player1BackgroundShadow.setFillColor(shadowColor);
	_texts.emplace_back(Text(
		sf::Vector2f(0, 0), // This will be set in OnPacketReceived
		{ TextLine({ CustomText{ .Text = "", .Size = 20 }}) }
	));

	_player2Icon = sf::RectangleShape(iconSizeF);
	_player2Icon.setTexture(defaultIcon);
	_player2Icon.setPosition(player2Position);
	_player2Icon.setScale(-1.f, 1.f);
	_player2IconShadow = sf::RectangleShape(iconSizeF);
	_player2IconShadow.setTexture(defaultIcon);
	_player2IconShadow.setPosition(player2Position + shadowOffset * 0.5f);
	_player2IconShadow.setFillColor(shadowColor);
	_player2IconShadow.setScale(-1.f, 1.f);
	_player2Background = sf::RectangleShape(iconSizeF);
	_player2Background.setTexture(player2Icon);
	_player2Background.setPosition(player2Position);
	_player2Background.setScale(-1.f, 1.f);
	_player2BackgroundShadow = sf::RectangleShape(iconSizeF);
	_player2BackgroundShadow.setTexture(player2Icon);
	_player2BackgroundShadow.setPosition(player2Position + shadowOffset);
	_player2BackgroundShadow.setFillColor(shadowColor);
	_player2BackgroundShadow.setScale(-1.f, 1.f);
	_texts.emplace_back(Text(
		sf::Vector2f(0, 0), // This will be set in OnPacketReceived
		{ TextLine({ CustomText{ .Text = "", .Size = 20 }}) }
	));

	// Create and place deck icons
	const auto& deckIconSize = sf::Vector2f(210, 200);
	const auto& decksWidth = _deckIcons.size() * deckIconSize.x;
	const auto& startPosition = sf::Vector2f(Game::WIDTH / 2.f - decksWidth / 2.f, Game::HEIGHT / 2.f + 100.f);

	for (std::size_t i = 0; i < _deckIcons.size(); i++)
	{
		const auto& position = startPosition + sf::Vector2f(i * deckIconSize.x, 0);
		const auto deckType = static_cast<DeckType>(i);

		_deckIcons[i].SetTexture(static_cast<CardType>(i));
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
		  Game::GetLobby().DeckType = deckType;
		  for (auto buttonIndex = 2; buttonIndex < _buttons.size(); ++buttonIndex)
		  {
			  _buttons[buttonIndex].Toggle(buttonIndex - 2 == static_cast<int>(deckType));
		  }

		  Game::SendPacket(new ChangeDeckPacket(deckType));
		});

		if (deckType == Game::GetLobby().DeckType) button.Toggle(true);

		button.Disable(); // Disable all buttons by default

		_buttons.emplace_back(button);
	}
}

void LobbyGui::OnPacketReceived(const Packet& packet)
{
	if (packet.type == PacketType::StartGame)
	{
		Game::SetState(GameState::GAME);
	}
	else if (packet.type == PacketType::LobbyInformation)
	{
		const auto& lobby = Game::GetLobby();

		if (lobby.WaitingForOpponent)
		{
			_buttons[0].Disable();
			_texts[1] = Text(
				sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
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
				sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
				{ TextLine({ CustomText{ .Text = "Ready to start !", .Size = 30 }}) }
			);
		}

		const auto& iconSize = _player1Icon.getSize();
		const auto& p1TextPosition = _player1Icon.getPosition() + sf::Vector2f(iconSize.x / 2.f, iconSize.y * 0.875f);
		const auto& p2TextPosition = _player2Icon.getPosition() + sf::Vector2f(-iconSize.x / 2.f, iconSize.y * 0.875f);
		const auto& p1Icon = &AssetManager::GetCardIcon(lobby.Player1.IconIndex);
		const auto& p2Icon = &AssetManager::GetCardIcon(lobby.Player2.IconIndex);

		_texts[2] = Text(
			p1TextPosition,
			{ TextLine({ CustomText{ .Text = lobby.Player1.Name, .Size = 20 }}) }
		);
		_texts[3] = Text(
			p2TextPosition,
			{ TextLine({ CustomText{ .Text = lobby.Player2.Name, .Size = 20 }}) }
		);

		_player1Icon.setTexture(p1Icon);
		_player1IconShadow.setTexture(p1Icon);
		_player2Icon.setTexture(p2Icon);
		_player2IconShadow.setTexture(p2Icon);
	}
}

void LobbyGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const auto& lobby = Game::GetLobby();

	if (!lobby.Player1.Name.empty())
	{
		target.draw(_player1BackgroundShadow, states);
		target.draw(_player1Background, states);
		target.draw(_player1IconShadow, states);
		target.draw(_player1Icon, states);
	}

	if (!lobby.Player2.Name.empty())
	{
		target.draw(_player2BackgroundShadow, states);
		target.draw(_player2Background, states);
		target.draw(_player2IconShadow, states);
		target.draw(_player2Icon, states);
	}

	if (lobby.IsHost && !lobby.WaitingForOpponent)
	{
		for (const auto& deckIcon : _deckIcons)
		{
			deckIcon.Draw(target, states);
		}
	}
}
