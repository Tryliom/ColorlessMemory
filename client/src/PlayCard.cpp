#include "PlayCard.h"

#include "AssetManager.h"

#include <utility>

PlayCard::PlayCard(DeckType type, CardIconIndex index)
{
	_index = index;
	_revealTime = 0;

	if (!AssetManager::IsInitialized()) return;

	_hiddenCardTexture = AssetManager::GetCardTexture(type, false);
	_cardTexture = AssetManager::GetCardTexture(type, true);

	if (index != UNKNOWN_ICON_INDEX) _iconTexture = AssetManager::GetCardIcon(index);
}

void PlayCard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const auto& globalBounds = _hiddenCardTexture.getSize();
	sf::Sprite sprite;

	sprite.setScale(_scale);
	sprite.setPosition(_position + sf::Vector2f(globalBounds.x / 2.f, 0));
	sprite.setOrigin(globalBounds.x / 2.f, 0);
	sprite.setColor(_color);

	sf::Sprite shadow = sprite;

	shadow.setColor(sf::Color(0, 0, 0, 100));
	shadow.move(0, 5 * _scale.y);

	if (_revealTime > 0.f)
	{
		auto factor = 1.f - _revealTime / _revealDuration;
		auto scale = 2.f * factor;
		auto scaleX = 1.f - scale;
		auto absScaleX = std::abs(scaleX);
		auto currentScaleX = _scale.x * absScaleX;

		sprite.setScale(currentScaleX, _scale.y);
		shadow.setScale(currentScaleX, _scale.y);

		target.draw(shadow, states);

		if (scaleX <= 0.f)
		{
			sprite.setTexture(_revealed ? _hiddenCardTexture : _cardTexture);
			target.draw(sprite, states);
		}
		else
		{
			sprite.setTexture(_revealed ? _cardTexture : _hiddenCardTexture);
			target.draw(sprite, states);
		}
	}
	else
	{
		target.draw(shadow, states);

		if (_revealed)
		{
			sprite.setTexture(_cardTexture);
			target.draw(sprite, states);
		}
		else
		{
			sprite.setTexture(_hiddenCardTexture);
			target.draw(sprite, states);
		}
	}

	bool displayIcon = _index != UNKNOWN_ICON_INDEX;
	bool inAnimation = _revealTime > 0.f;
	float percentage = 1.f - _revealTime / _revealDuration;

	if (displayIcon && _revealed && inAnimation && percentage > 0.5f)
	{
		displayIcon = false;
	}

	if (displayIcon && !_revealed && (!inAnimation || percentage < 0.5f))
	{
		displayIcon = false;
	}

	if (displayIcon)
	{
		sf::Sprite iconSprite;
		iconSprite.setScale(sprite.getScale());
		iconSprite.setTexture(_iconTexture);

		const auto& iconTextureBounds = _iconTexture.getSize();

		sprite.setTexture(_iconTexture);
		sprite.setPosition(_position + sf::Vector2f(iconTextureBounds.x / 2.f, 0));
		sprite.setOrigin(iconTextureBounds.x / 2.f, 0);

		shadow = sprite;
		shadow.setColor(sf::Color(0, 0, 0, 100));
		shadow.move(0, 5 * _scale.y);

		target.draw(shadow, states);
		target.draw(sprite, states);
	}
}

void PlayCard::Update(sf::Time elapsed)
{
	if (_revealTime > 0.f)
	{
		_revealTime -= elapsed.asSeconds();

		if (_revealTime <= 0.f)
		{
			_revealTime = 0.f;
			_revealed = !_revealed;

			OnHover(_hover);
		}
	}
}

void PlayCard::OnHover(bool hover)
{
	if (_disabled || _revealed) return;

	if (hover)
	{
		_hover = true;
		_color = sf::Color(255, 255, 255, 255);
	}
	else
	{
		const auto& rgb = 200;
		_color = sf::Color(rgb, rgb, rgb, 255);
		_hover = false;
	}
}

void PlayCard::SetIndex(CardIconIndex index)
{
	_index = index;
	_iconTexture = AssetManager::GetCardIcon(index);
}

void PlayCard::SetOnClicked(std::function<void()> onClicked)
{
	_onClick = std::move(onClicked);
}

void PlayCard::SetPosition(sf::Vector2f position)
{
	_position = position;
}

void PlayCard::SetScale(float scale)
{
	_scale = sf::Vector2f(scale, scale);
}

void PlayCard::StartFlip()
{
	_revealTime = _revealDuration;
	_color = sf::Color(255, 255, 255, 255);
}

void PlayCard::Click()
{
	if (_onClick) _onClick();
}

void PlayCard::Disable()
{
	_onClick = nullptr;
	_disabled = true;
	_color = sf::Color(255, 255, 255, 255);
}

bool PlayCard::IsHover() const
{
	return _hover;
}

bool PlayCard::IsRevealed() const
{
	return _revealed;
}

bool PlayCard::IsFlipping() const
{
	return _revealTime > 0.f;
}

sf::FloatRect PlayCard::GetGlobalBounds() const
{
	sf::Sprite sprite;
	sprite.setPosition(_position + sf::Vector2f(_hiddenCardTexture.getSize().x / 2.f, 0));
	sprite.setScale(_scale);
	sprite.setTexture(_cardTexture);
	sprite.setOrigin(_cardTexture.getSize().x / 2.f, 0);

	return sprite.getGlobalBounds();
}

CardIconIndex PlayCard::GetIconIndex() const
{
	return _index;
}
