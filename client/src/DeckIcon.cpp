#include "DeckIcon.h"

void DeckIcon::SetTexture(DeckType deckType)
{
	if (!AssetManager::IsInitialized()) return;

	const auto& card = &AssetManager::GetCardTexture(deckType, false);
	const auto& cardSize = card->getSize();
	const auto& cardSizeF = sf::Vector2f(cardSize.x, cardSize.y);
	const auto& scale = 0.3f;

	_card1.setTexture(card);
	_card2.setTexture(card);
	_card1.setSize(cardSizeF);
	_card2.setSize(cardSizeF);
	_card1.setScale(scale, scale);
	_card2.setScale(scale, scale);
}

void DeckIcon::SetPosition(sf::Vector2f position)
{
	position.x -= _card1.getSize().x / 2.f * _card1.getScale().x;

	_card1.setPosition(position);
	_card2.setPosition(position + sf::Vector2f(3, 3));
}

void DeckIcon::Draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_card2, states);
	target.draw(_card1, states);
}