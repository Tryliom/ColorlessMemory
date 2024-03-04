#pragma once

#include "gui/Gui.h"

class GameGui final : public Gui
{
public:
	explicit GameGui();

	void OnUpdate(sf::Time elapsed) override;
};