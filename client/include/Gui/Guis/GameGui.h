#pragma once

#include "gui/Gui.h"
#include "PlayCard.h"
#include "PlayerUi.h"

class Game;
class GameManager;

class GameGui final :
	public Gui
{
 private:
	Game& _game;
	GameManager& _gameManager;

	float _height = 0;
	float _width = 0;

	std::vector<PlayCard> _playCards;
	std::size_t _xCards = 0;
	std::size_t _yCards = 0;

	// Players
	PlayerUi _player1;
	PlayerUi _player2;
	bool _yourTurn{};

	bool _blockInput = false;
	float _endTurnTimer = 0.f;
	float _beforeScoringTimer = 0.f;

	bool _gameOver = false;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnCheckInputs(sf::Event event) override;

	void OnSelectPlayCard(CardIndex cardIndex);
	void SelectCard(CardIndex cardIndex);
	void StartTurn();

	void UpdateScoringTimer(sf::Time elapsed);
	void UpdateEndTimer(sf::Time elapsed);

 public:
	explicit GameGui(Game& game, GameManager& gameManager, float width, float height);

	void OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition) override;
	void OnPacketReceived(Packet& packet) override;
};