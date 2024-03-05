#pragma once

#include "gui/Gui.h"

class LobbyGui final : public Gui
{
	bool _ready{ false };

public:
	explicit LobbyGui();

	void OnUpdate(sf::Time elapsed) override;
};