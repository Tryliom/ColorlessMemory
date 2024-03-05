#include "gui/Gui.h"

#include "Game.h"

Gui::Gui()
{
	_background.setSize(sf::Vector2f(Game::WIDTH, Game::HEIGHT));
	_background.setFillColor(sf::Color(0, 0, 0, 100));
}

void Gui::draw(sf::RenderTarget& target, const sf::RenderStates states) const
{
	target.draw(_background, states);

	OnDraw(target, states);

	for (auto& button : _buttons)
	{
		if (!button.IsDisabled())
		{
			target.draw(button, states);
		}
	}

	for (auto& text : _texts)
	{
		target.draw(text, states);
	}

}

void Gui::Update(const sf::Time elapsed)
{
	const auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(Game::GetWindow()));

	for (auto& button : _buttons)
	{
		if (button.IsDisabled())
		{
			continue;
		}

		button.Update(elapsed);

		if (button.GetGlobalBounds().contains(mousePosition))
		{
			if (!button.IsHover())
			{
				button.OnStartHover();
			}
		}
		else
		{
			if (button.IsHover())
			{
				button.OnEndHover();
			}
		}
	}

	OnUpdate(elapsed);
}

void Gui::CheckInputs(const sf::Event event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			for (auto& button : _buttons)
			{
				if (button.IsHover() && !button.IsDisabled())
				{
					button.OnClick();
					break;
				}
			}
		}
	}
}
