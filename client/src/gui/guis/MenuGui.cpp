#include "gui/guis/MenuGui.h"

#include "Game.h"
#include "AssetManager.h"

MenuGui::MenuGui()
{
	// Create buttons
	auto playButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT * 3.f / 4.f), 
		sf::Vector2f(200, 50),
		true
	);

	playButton.SetText({
		TextLine({CustomText{ .Text = "PLAY", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	playButton.SetOnClick([]() {
		Game::SetState(GameState::LOBBY);
	});

	_buttons.emplace_back(playButton);

	auto quitButton = Button(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT - 100.f),
		sf::Vector2f(200, 50),
		true
	);

	quitButton.SetText({
		TextLine({CustomText{.Text = "QUIT", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	quitButton.SetOnClick([]() {
		Game::Exit();
	});

	_buttons.emplace_back(quitButton);

	auto width = Game::WIDTH * 0.75f;
	auto xStart = Game::WIDTH / 2.f - width / 2.f + 42.f;
	auto iconIndex = Game::GetPlayer().IconIndex;
	auto cardSize = AssetManager::GetCardIcon(0).getSize();
	auto iconSize = sf::Vector2f(cardSize.x, cardSize.y);

	for (std::size_t i = 0; i < _iconIndexes.size(); ++i)
	{
		auto iconButton = Button(
			sf::Vector2f(xStart + i * (width / _iconIndexes.size()) + iconSize.x / 2.f, Game::HEIGHT / 2.f + 100.f),
			sf::Vector2f(200, 50),
			true
		);

		iconButton.SetText({
			TextLine({CustomText{.Text = "Icon " + std::to_string(i + 1), .Size = 24}})
		});
		iconButton.SetOnClick([i, this]() {
			for (auto buttonIndex = 2; buttonIndex < _buttons.size(); ++buttonIndex)
			{
				_buttons[buttonIndex].Toggle(buttonIndex - 2 == i);
			}

			Game::GetPlayer().IconIndex = _iconIndexes[i];
		});

		if (iconIndex == _iconIndexes[i] || (iconIndex == 0 && i == 0))
		{
			iconButton.Toggle(true);
		}

		_buttons.emplace_back(iconButton);

		// Add icon image
		_icons[i] = sf::RectangleShape(iconSize);
		_icons[i].setTexture(&AssetManager::GetCardIcon(_iconIndexes[i]));
		_icons[i].setPosition(xStart + i * (width / _iconIndexes.size()), Game::HEIGHT / 2.f - 200.f);

		// Add background
		_backgrounds[i] = sf::RectangleShape(sf::Vector2f(iconSize.x, iconSize.y * 0.7f));
		_backgrounds[i].setFillColor(sf::Color(0, 0, 0, 50));
		_backgrounds[i].setPosition(xStart + i * (width / _iconIndexes.size()), Game::HEIGHT / 2.f - 217.f + iconSize.y * 0.3f);
	}

	// Create texts
	auto title = Text(
		sf::Vector2f(Game::WIDTH / 2.f, 100.f),
		{
			TextLine({CustomText{.Text = "Colorless Memory", .Size = 50}}),
			TextLine({CustomText{.Text = "By Tryliom", .Style = sf::Text::Style::Italic, .Size = 16}})
		}
	);

	_texts.emplace_back(title);
	_texts.emplace_back(Text(
		sf::Vector2f(Game::WIDTH / 2.f, Game::HEIGHT / 2.f - 200.f),
		{
			TextLine({CustomText{.Text = "Choose your icon", .Size = 30}})
		}
	));
}

void MenuGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto& background : _backgrounds)
	{
		target.draw(background, states);
	}

	for (auto& icon : _icons)
	{
		target.draw(icon, states);
	}
}
