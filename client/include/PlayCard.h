#pragma once

#include "Constants.h"

#include <SFML/Graphics.hpp>

#include <functional>

class PlayCard : public sf::Drawable
{
 private:
	CardIconIndex _index;
	sf::Vector2f _position;
	sf::Vector2f _scale = {1, 1};
	sf::Color _color = sf::Color(200, 200, 200, 255);
	bool _hover = false;
	bool _disabled = false;

	sf::Texture _hiddenCardTexture;
	sf::Texture _cardTexture;
	sf::Texture _iconTexture;

	std::function<void()> _onClick;

	// Animation
	bool _revealed = false;
	float _revealTime;
	constexpr static float _revealDuration = 0.5f;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 public:
	PlayCard(DeckType type, CardIconIndex index);

	void Update(sf::Time elapsed);
	void OnHover(bool hover);

	void SetIndex(CardIconIndex index);
	void SetOnClicked(std::function<void()> onClicked);
	void SetPosition(sf::Vector2f position);
	void SetScale(float scale);

	void StartFlip();
	void Click();
	void Disable();

	CardIconIndex GetIconIndex() const;
	bool IsHover() const;
	bool IsRevealed() const;
	bool IsFlipping() const;
	sf::FloatRect GetGlobalBounds() const;
};