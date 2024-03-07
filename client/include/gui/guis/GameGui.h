#pragma once

#include "gui/Gui.h"
#include "PlayCard.h"
#include "PlayerUi.h"

class GameGui final :
	public Gui
{
 private:
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

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnCheckInputs(sf::Event event) override;

	void OnSelectPlayCard(std::size_t i);
	void SelectCard(std::size_t i);
	void StartTurn();

 public:
	explicit GameGui();

	void OnUpdate(sf::Time elapsed) override;
	void OnPacketReceived(const Packet& packet) override;
};