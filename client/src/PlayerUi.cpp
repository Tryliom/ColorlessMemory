#include "PlayerUi.h"

#include "AssetManager.h"

PlayerUi::PlayerUi(bool isPlayer1, sf::Vector2f position, bool displayScore)
{
	auto iconSize = sf::Vector2f(100, 100);;
	const auto& shadowOffset = sf::Vector2f(0, 5);
	const auto& shadowColor = sf::Color(0, 0, 0, 100);

	_playerIcon = sf::RectangleShape(iconSize);
	_playerIcon.setPosition(position);

	_playerIconShadow = sf::RectangleShape(iconSize);
	_playerIconShadow.setPosition(position + shadowOffset * 0.5f);
	_playerIconShadow.setFillColor(shadowColor);

	_playerBackground = sf::RectangleShape(iconSize);
	_playerBackground.setPosition(position);

	_playerBackgroundShadow = sf::RectangleShape(iconSize);
	_playerBackgroundShadow.setPosition(position + shadowOffset);
	_playerBackgroundShadow.setFillColor(shadowColor);

	if (AssetManager::IsInitialized())
	{
		const auto& defaultIcon = &AssetManager::GetCardIcon(DEFAULT_ICON_INDEX);
		const auto& player1Icon = &AssetManager::GetTexture(TextureType::PLAYER1_ICON_BACKGROUND);
		const auto& player2Icon = &AssetManager::GetTexture(TextureType::PLAYER2_ICON_BACKGROUND);

		iconSize = sf::Vector2f(defaultIcon->getSize());

		_playerIcon.setTexture(defaultIcon);
		_playerIconShadow.setTexture(defaultIcon);
		_playerBackground.setTexture(isPlayer1 ? player1Icon : player2Icon);
		_playerBackgroundShadow.setTexture(isPlayer1 ? player1Icon : player2Icon);
	}

	_playerNamePosition = position + sf::Vector2f(iconSize.x / 2.f, iconSize.y * 0.875f);

	if (!isPlayer1)
	{
		_playerNamePosition.x -= iconSize.x;
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
			_playerScorePosition.x -= iconSize.x;
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

void PlayerUi::SetIcon(CardIconIndex iconIndex)
{
	if (!AssetManager::IsInitialized()) return;

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