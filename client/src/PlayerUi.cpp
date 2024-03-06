#include "PlayerUi.h"

#include "AssetManager.h"

PlayerUi::PlayerUi(bool isPlayer1, sf::Vector2f position, bool displayScore)
{
	const auto& defaultIcon = &AssetManager::GetCardIcon(0);
	const auto& player1Icon = &AssetManager::GetTexture(TextureType::PLAYER1_ICON_BACKGROUND);
	const auto& player2Icon = &AssetManager::GetTexture(TextureType::PLAYER2_ICON_BACKGROUND);
	const auto& iconSize = defaultIcon->getSize();
	const auto& iconSizeF = sf::Vector2f(iconSize.x, iconSize.y);
	const auto& shadowOffset = sf::Vector2f(0, 5);
	const auto& shadowColor = sf::Color(0, 0, 0, 100);

	_playerIcon = sf::RectangleShape(iconSizeF);
	_playerIcon.setTexture(defaultIcon);
	_playerIcon.setPosition(position);

	_playerIconShadow = sf::RectangleShape(iconSizeF);
	_playerIconShadow.setTexture(defaultIcon);
	_playerIconShadow.setPosition(position + shadowOffset * 0.5f);
	_playerIconShadow.setFillColor(shadowColor);

	_playerBackground = sf::RectangleShape(iconSizeF);
	_playerBackground.setTexture(isPlayer1 ? player1Icon : player2Icon);
	_playerBackground.setPosition(position);

	_playerBackgroundShadow = sf::RectangleShape(iconSizeF);
	_playerBackgroundShadow.setTexture(isPlayer1 ? player1Icon : player2Icon);
	_playerBackgroundShadow.setPosition(position + shadowOffset);
	_playerBackgroundShadow.setFillColor(shadowColor);

	_playerNamePosition = position + sf::Vector2f(iconSize.x / 2.f, iconSize.y * 0.875f);

	if (!isPlayer1)
	{
		_playerNamePosition.x -= iconSizeF.x;
		_playerIcon.setScale(-1.f, 1.f);
		_playerIconShadow.setScale(-1.f, 1.f);
		_playerBackground.setScale(-1.f, 1.f);
		_playerBackgroundShadow.setScale(-1.f, 1.f);
	}

	_playerName = Text(
		_playerNamePosition,
		{
			TextLine({ CustomText{ .Text = "", .Size = 20 }})
		}
	);

	if (displayScore)
	{
		_playerScorePosition = position + sf::Vector2f(iconSize.x / 2.f, iconSize.y * 0.125f);

		if (!isPlayer1)
		{
			_playerScorePosition.x -= iconSizeF.x;
		}

		_playerScore = Text(
			_playerScorePosition,
			{
				TextLine({ CustomText{ .Text = "0", .Size = 20 }})
			}
		);
	}
	else
	{
		_playerScore = Text(
			_playerScorePosition,
			{
				TextLine({ CustomText{ .Text = "", .Size = 20 }})
			}
		);
	}
}

void PlayerUi::SetName(const std::string& name)
{
	_playerName = Text(
		_playerNamePosition,
		{
			TextLine({ CustomText{ .Text = name, .Size = 20 }})
		}
	);
}

void PlayerUi::SetScore(int score)
{
	_playerScore = Text(
		_playerScorePosition,
		{
			TextLine({ CustomText{ .Text = "Score: " + std::to_string(score), .Size = 20 }})
		}
	);
}

void PlayerUi::SetIcon(std::size_t iconIndex)
{
	_playerIcon.setTexture(&AssetManager::GetCardIcon(iconIndex));
	_playerIconShadow.setTexture(&AssetManager::GetCardIcon(iconIndex));
}

void PlayerUi::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_playerBackgroundShadow, states);
	target.draw(_playerBackground, states);
	target.draw(_playerIconShadow, states);
	target.draw(_playerIcon, states);
	target.draw(_playerName, states);
	target.draw(_playerScore, states);
}