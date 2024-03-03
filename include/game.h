#pragma once

#include <SFML/Graphics.hpp>

enum class GameState
{
	NONE,
	MAIN_MENU
};

namespace Game
{
	inline static const float PI = 3.14159265359f;
	inline static float HEIGHT = 0.f;
	inline static float WIDTH = 0.f;

	void Initialize();
	int Loop();
	sf::RenderWindow& GetWindow();
	void SetState(GameState state);
	void Exit();
}