#include "PlayCard.h"

#include "AssetManager.h"

#include <utility>

PlayCard::PlayCard(DeckType type, int index)
{
	_index = index;
	_hiddenCardTexture = AssetManager::GetCardTexture(type, false);
	_cardTexture = AssetManager::GetCardTexture(type, true);
	_revealTime = 0;

	if (index != -1) _iconTexture = AssetManager::GetCardIcon(index);
}

void PlayCard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::Sprite sprite;
	sprite.setPosition(_position);
	sprite.setScale(_scale);
	sprite.setColor(_color);

	sf::Sprite shadow = sprite;

	shadow.setColor(sf::Color(0, 0, 0, 100));
	shadow.move(0, 5 * _scale.y);

	if (_revealTime > 0.f)
	{
		float factor = 1.f - _revealTime / _revealDuration;
		float angle = 180.f * factor;
		sprite.setRotation(angle);
		shadow.setRotation(angle);

		target.draw(shadow, states);

		if (angle > 90.f)
		{
			sprite.setTexture(_revealed ? _cardTexture : _hiddenCardTexture);
			target.draw(sprite, states);
		}
		else
		{
			sprite.setTexture(_revealed ? _hiddenCardTexture : _cardTexture);
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

	if (_index != -1)
	{
		sprite.setTexture(_iconTexture);
		sprite.setPosition(_position + sf::Vector2f(5, 5));
		sprite.setScale(_scale);

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
		}
	}
}

void PlayCard::OnHover(bool hover)
{
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

void PlayCard::SetIndex(int index)
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

void PlayCard::StartFlip(float time)
{
	_revealTime = time;
}

bool PlayCard::HasIcon() const
{
	return _index != -1;
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
	sprite.setPosition(_position);
	sprite.setScale(_scale);
	sprite.setTexture(_cardTexture);

	return sprite.getGlobalBounds();
}
