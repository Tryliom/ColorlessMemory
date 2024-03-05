#include "gui/guis/LobbyGui.h"

#include "Game.h"
#include "Logger.h"
#include "Packet.h"
#include "AssetManager.h"

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
			{TextLine({ CustomText{ .Text = "Lobby", .Size = 50 }})}
	);
	auto waiting = Text(
			sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
			{TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }})}
	);

	_texts.emplace_back(title);
	_texts.emplace_back(waiting);

	const auto& defaultIcon = &AssetManager::GetCardIcon(0);
	const auto& iconSize = defaultIcon->getSize();
	const auto& iconSizeF = sf::Vector2f(iconSize.x, iconSize.y);

	// Create player icons and names for each side
	_player1Icon = sf::RectangleShape(iconSizeF);
	_player1Icon.setTexture(defaultIcon);
	_player1Icon.setPosition(100, Game::HEIGHT / 2.f - 200.f);
	_player1Background = sf::RectangleShape(iconSizeF);
	_player1Background.setFillColor(sf::Color(0, 0, 0, 100));
	_player1Background.setPosition(100, Game::HEIGHT / 2.f - 140.f);
	_player1Background.setOutlineColor(sf::Color::White);
	_player1Background.setOutlineThickness(2.f);
	_texts.emplace_back(Text(
			sf::Vector2f(100 + iconSizeF.x, Game::HEIGHT / 2.f - 200.f + iconSizeF.y + 10),
			{ TextLine({ CustomText{ .Text = "", .Size = 20 }}) }
	));

	_player2Icon = sf::RectangleShape(iconSizeF);
	_player2Icon.setTexture(defaultIcon);
	_player2Icon.setPosition(Game::WIDTH - 100, Game::HEIGHT / 2.f - 200.f);
	_player2Icon.setScale(-1.f, 1.f);
	_player2Background = sf::RectangleShape(iconSizeF);
	_player2Background.setFillColor(sf::Color(0, 0, 0, 100));
	_player2Background.setPosition(Game::WIDTH - 100 - 205, Game::HEIGHT / 2.f - 140.f);
	_player2Background.setOutlineColor(sf::Color::White);
	_player2Background.setOutlineThickness(2.f);
	_texts.emplace_back(Text(
			sf::Vector2f(Game::WIDTH - 100 - iconSizeF.x / 2.f, Game::HEIGHT / 2.f - 200.f + iconSizeF.y + 10),
			{ TextLine({ CustomText{ .Text = "", .Size = 20 }}) }
	));
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
			if (lobby.IsHost) _buttons[0].Enable();
			else _buttons[0].Disable();
			_texts[1] = Text(
					sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
					{ TextLine({ CustomText{ .Text = "Ready to start !", .Size = 30 }}) }
			);
		}

		const auto& iconSize = _player1Icon.getSize();

		_texts[2] = Text(
				sf::Vector2f(100 + iconSize.x / 2.f, Game::HEIGHT / 2.f - 200.f + iconSize.y + 10),
				{ TextLine({ CustomText{ .Text = lobby.Player1.Name, .Size = 20 }}) }
		);
		_texts[3] = Text(
				sf::Vector2f(Game::WIDTH - 100 - iconSize.x / 2.f, Game::HEIGHT / 2.f - 200.f + iconSize.y + 10),
				{ TextLine({ CustomText{ .Text = lobby.Player2.Name, .Size = 20 }}) }
		);

		_player1Icon.setTexture(&AssetManager::GetCardIcon(lobby.Player1.IconIndex));
		_player2Icon.setTexture(&AssetManager::GetCardIcon(lobby.Player2.IconIndex));
	}
}

void LobbyGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const auto& lobby = Game::GetLobby();

	if (!lobby.Player1.Name.empty())
	{
		target.draw(_player1Background, states);
		target.draw(_player1Icon, states);
	}

	if (!lobby.Player2.Name.empty())
	{
		target.draw(_player2Background, states);
		target.draw(_player2Icon, states);
	}
}
