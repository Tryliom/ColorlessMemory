#include "gui/guis/LobbyGui.h"

#include "Game.h"
#include "Logger.h"
#include "Packet.h"

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
			{
					TextLine({ CustomText{ .Text = "Lobby", .Size = 50 }})
			},
			-1,
			true
	);
	auto waiting = Text(
			sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f),
			{
					TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }})
			},
			-1,
			true
	);

	_texts.emplace_back(title);
	_texts.emplace_back(waiting);
}

void LobbyGui::OnPacketReceived(const Packet& packet)
{
	if (packet.type == PacketType::StartGame)
	{
		Game::SetState(GameState::GAME);
	}
	else if (packet.type == PacketType::JoinLobby)
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
	}
}
