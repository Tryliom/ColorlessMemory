#include "gui/guis/GameGui.h"

#include "Game.h"

GameGui::GameGui()
{
	// Create texts
	auto title = Text(
		sf::Vector2f(Game::WIDTH / 2.f, 100.f),
		{
			TextLine({CustomText{.Text = "Game", .Size = 50}}),
			TextLine({CustomText{.Text = "In construction", .Size = 40}}),
		}
	);

	_texts.emplace_back(title);
}

void GameGui::OnUpdate(sf::Time elapsed)
{

}
