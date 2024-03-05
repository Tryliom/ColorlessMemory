#pragma once

#include "gui/Gui.h"

class LobbyGui final : public Gui
{
public:
	explicit LobbyGui();

	void OnPacketReceived(const Packet& packet) override;
};