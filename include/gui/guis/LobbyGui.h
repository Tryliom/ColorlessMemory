#pragma once

#include "gui/Gui.h"

class LobbyGui final : public Gui
{
public:
	explicit LobbyGui();

	void OnUpdate(sf::Time elapsed) override;
};