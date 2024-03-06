#include "PlayCard.h"

#include <utility>

PlayCard::PlayCard(CardType type, int index)
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

	if (_revealTime > 0.f)
	{
		float factor = 1.f - _revealTime / _revealDuration;
		float angle = 180.f * factor;
		sprite.setRotation(angle);

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
		sprite.setRotation(0);

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
		_color = sf::Color(255, 255, 255, 255);
	}
	else
	{
		const auto& rgb = 230;
		_color = sf::Color(rgb, rgb, rgb, 255);
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

sf::FloatRect PlayCard::GetGlobalBounds() const
{
	sf::Sprite sprite;
	sprite.setPosition(_position);
	sprite.setScale(_scale);
	sprite.setTexture(_cardTexture);

	return sprite.getGlobalBounds();
}
