#pragma once

#include "AssetManager.h"

#include <functional>

class PlayCard : public sf::Drawable
{
 private:
	int _index;
	sf::Vector2f _position;
	sf::Vector2f _scale;
	sf::Color _color;

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
	PlayCard(CardType type, int index);

	void Update(sf::Time elapsed);
	void OnHover(bool hover);

	void SetIndex(int index);
	void SetOnClicked(std::function<void()> onClicked);
	void SetPosition(sf::Vector2f position);
	void SetScale(float scale);

	void StartFlip(float time);
	bool HasIcon() const;
	sf::FloatRect GetGlobalBounds() const;
};