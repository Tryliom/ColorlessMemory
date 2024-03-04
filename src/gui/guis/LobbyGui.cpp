#include "gui/guis/LobbyGui.h"

#include "Game.h"

LobbyGui::LobbyGui()
{
	// Create buttons
	auto launchButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT * 3.f / 4.f), 
		sf::Vector2f(300, 50),
		true
	);

	launchButton.SetText({
		TextLine({CustomText{ .Text = "START GAME", .Style = sf::Text::Style::Bold, .Size = 30}})
	});
	launchButton.SetOnClick([]() {
		//TODO: Start game
	});

	_buttons.emplace_back(launchButton);

	auto returnButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT - 100.f),
		sf::Vector2f(200, 50),
		true
	);

	returnButton.SetText({
		TextLine({CustomText{.Text = "RETURN", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	returnButton.SetOnClick([]() {
		Game::SetState(GameState::MAIN_MENU);
	});

	_buttons.emplace_back(returnButton);

	// Create texts
	auto title = Text(
		sf::Vector2f(Game::WIDTH / 2.f, 100.f),
		{
			TextLine({CustomText{.Text = "Lobby", .Size = 50}}),
			TextLine({CustomText{.Text = "Waiting for players...", .Size = 40}}),
		},
		-1,
		true
	);

	_texts.emplace_back(title);
}

void LobbyGui::OnUpdate(sf::Time elapsed)
{

}
