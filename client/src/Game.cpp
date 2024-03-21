#include "Game.h"

#include "MyPackets.h"
#include "MyPackets/LobbyInformationPacket.h"

#include "Logger.h"

#include "AssetManager.h"
#include "gui/guis/MenuGui.h"
#include "gui/guis/LobbyGui.h"
#include "gui/guis/GameGui.h"

#include <SFML/Graphics.hpp>

#include <cstdlib>

Game::Game(GameManager& gameManager) : _gameManager(gameManager)
{
	SetBackground(AssetManager::GetTexture(TextureType::BACKGROUND_MENU));
	SetState(GameState::MAIN_MENU);
}

void Game::CheckInputs(const sf::Event& event)
{
	if (_gui != nullptr)
	{
		_gui->CheckInputs(event);
	}
}

void Game::Update(sf::Time elapsed)
{
	if (_gui != nullptr)
	{
		_gui->Update(elapsed);
	}
}

void Game::SetState(GameState state)
{
	if (_state == state) return;

	delete _gui;

	if (state == GameState::MAIN_MENU)
	{
		_gui = new MenuGui();
	}

	if (state == GameState::LOBBY)
	{
		_gui = new LobbyGui();
		_gameManager.JoinLobby();
		//TODO: Add network interface for this
		SendPacket(new JoinLobbyPacket(_player.Name, _player.IconIndex));
	}

	if (state == GameState::GAME)
	{
		_gui = new GameGui();
	}

	_state = state;
}

void Game::Draw(sf::RenderTarget& target)
{
	// Render background
	target.draw(_background);

	if (_gui != nullptr)
	{
		target.draw(*_gui);
	}
}

void Game::OnPacketReceived(Packet& packet)
{
	if (_gui != nullptr)
	{
		_gui->OnPacketReceived(packet);
	}
}

void Game::SetBackground(const sf::Texture& texture)
{
	_background.setTexture(&texture);
	_background.setSize(sf::Vector2f(texture.getSize()));
	_background.setPosition(0, 0);
}