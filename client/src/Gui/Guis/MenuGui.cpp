#include "gui/guis/MenuGui.h"

#include "Game.h"
#include "AssetManager.h"

MenuGui::MenuGui(Game& game, GameManager& gameManager, float width, float height)
{
	// Create buttons
	auto playButton = Button(
		sf::Vector2f(width / 2.f, height * 3.f / 4.f), 
		sf::Vector2f(200, 50),
		true
	);

	playButton.SetText({
		TextLine({CustomText{ .Text = "PLAY", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	playButton.SetOnClick([&game]() {
		game.SetState(GameState::LOBBY);
	});

	_buttons.emplace_back(playButton);

	auto quitButton = Button(
		sf::Vector2f(width / 2.f, height - 100.f),
		sf::Vector2f(200, 50),
		true
	);

	quitButton.SetText({
		TextLine({CustomText{.Text = "QUIT", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	quitButton.SetOnClick([&game]() {
		game.Quit();
	});

	_buttons.emplace_back(quitButton);

	auto widthSpace = width * 0.75f;
	auto xStart = width / 2.f - widthSpace / 2.f + 42.f;
	auto iconIndex = gameManager.GetPlayer().IconIndex;
	auto cardSize = AssetManager::GetCardIcon(DEFAULT_ICON_INDEX).getSize();
	auto iconSize = sf::Vector2f(cardSize.x, cardSize.y);

	for (std::size_t i = 0; i < _iconIndexes.size(); ++i)
	{
		auto iconButton = Button(
			sf::Vector2f(xStart + i * (widthSpace / _iconIndexes.size()) + iconSize.x / 2.f, height / 2.f + 100.f),
			sf::Vector2f(200, 50),
			true
		);

		iconButton.SetText({
			TextLine({CustomText{.Text = "Icon " + std::to_string(i + 1), .Size = 24}})
		});
		iconButton.SetOnClick([&gameManager, i, this]() {
			for (auto buttonIndex = 2; buttonIndex < _buttons.size(); ++buttonIndex)
			{
				_buttons[buttonIndex].Toggle(buttonIndex - 2 == i);
			}

			gameManager.SetPlayerIcon(_iconIndexes[i]);
		});

		if (iconIndex == _iconIndexes[i] || (iconIndex == IconType::Icon1 && i == 0))
		{
			iconButton.Toggle(true);
		}

		_buttons.emplace_back(iconButton);

		const auto& cardIcon = &AssetManager::GetCardIcon({ static_cast<char>(_iconIndexes[i]) });
		const auto& iconBackground = &AssetManager::GetTexture(TextureType::SIMPLE_ICON_BACKGROUND);
		const auto& shadowOffset = sf::Vector2f(0, 5);
		const auto& shadowColor = sf::Color(0, 0, 0, 100);

		// Add icon image
		_icons[i] = sf::RectangleShape(iconSize);
		_icons[i].setTexture(cardIcon);
		_icons[i].setPosition(xStart + i * (widthSpace / _iconIndexes.size()), height / 2.f - 200.f);
		_iconShadows[i] = sf::RectangleShape(iconSize);
		_iconShadows[i].setTexture(cardIcon);
		_iconShadows[i].setPosition(_icons[i].getPosition() + shadowOffset * 0.5f);
		_iconShadows[i].setFillColor(shadowColor);

		// Add background
		_backgrounds[i] = sf::RectangleShape(iconSize);
		_backgrounds[i].setTexture(iconBackground);
		_backgrounds[i].setPosition(_icons[i].getPosition());
		_backgroundShadows[i] = sf::RectangleShape(iconSize);
		_backgroundShadows[i].setTexture(iconBackground);
		_backgroundShadows[i].setPosition(_icons[i].getPosition() + shadowOffset);
		_backgroundShadows[i].setFillColor(shadowColor);
	}

	// Create texts
	auto title = Text(
		sf::Vector2f(width / 2.f, 100.f),
		{
			TextLine({CustomText{.Text = "Colorless Memory", .Size = 50}}),
			TextLine({CustomText{.Text = "By Tryliom", .Style = sf::Text::Style::Italic, .Size = 16}})
		}
	);

	_texts.emplace_back(title);
	_texts.emplace_back(Text(
		sf::Vector2f(width / 2.f, height / 2.f - 200.f),
		{
			TextLine({CustomText{.Text = "Choose your icon", .Size = 30}})
		}
	));
}

void MenuGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto& backgroundShadow : _backgroundShadows)
	{
		target.draw(backgroundShadow, states);
	}

	for (auto& background : _backgrounds)
	{
		target.draw(background, states);
	}

	for (auto& shadow : _iconShadows)
	{
		target.draw(shadow, states);
	}

	for (auto& icon : _icons)
	{
		target.draw(icon, states);
	}
}
