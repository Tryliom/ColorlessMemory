#include "Game.h"

#include "MyPackets.h"

#include "AssetManager.h"
#include "gui/guis/MenuGui.h"
#include "gui/guis/LobbyGui.h"
#include "gui/guis/GameGui.h"

#include <SFML/Graphics.hpp>
#include <utility>

Game::Game(GameManager& gameManager, ClientNetworkInterface& clientNetworkInterface, int width, int height) :
	_gameManager(gameManager), _networkManager(clientNetworkInterface), _width(width), _height(height)
{
	if (AssetManager::IsInitialized())
	{
		SetBackground(AssetManager::GetTexture(TextureType::BACKGROUND_MENU));
	}

	SetState(GameState::MAIN_MENU);
}

void Game::CheckInputs(const sf::Event& event)
{
	if (_gui != nullptr)
	{
		_gui->CheckInputs(event);
	}
}

void Game::Update(sf::Time elapsed, sf::Vector2f mousePosition)
{
	if (_gui != nullptr)
	{
		_gui->Update(elapsed, mousePosition);
	}

	while (Packet* packet = _networkManager.PopPacket())
	{
		_gameManager.OnPacketReceived(*packet);
		OnPacketReceived(*packet);

		auto packetTypeValue = packet->Type;

		if (packetTypeValue >= 0 && packetTypeValue <= static_cast<char>(MyPackets::MyPacketType::COUNT))
		{
			delete packet;
		}
	}
}

void Game::SetState(GameState state)
{
	if (_state == state) return;

	delete _gui;

	if (state == GameState::MAIN_MENU)
	{
		_gui = new MenuGui(*this, _gameManager, _width, _height);
	}

	if (state == GameState::LOBBY)
	{
		_gui = new LobbyGui(*this, _gameManager, _width, _height);
		_gameManager.JoinLobby();
		_networkManager.SendPacket(_gameManager.ToJoinLobbyPacket());
	}

	if (state == GameState::GAME)
	{
		_gui = new GameGui(*this, _gameManager, _width, _height);
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

void Game::SendPacket(Packet* packet)
{
	_networkManager.SendPacket(packet);
}

void Game::OnPacketReceived(Packet& packet)
{
	if (_gui != nullptr)
	{
		_gui->OnPacketReceived(packet);
	}
}

void Game::Quit()
{
	if (_onQuit != nullptr)
	{
		_onQuit();
	}
}

void Game::OnQuit(std::function<void()> onQuit)
{
	_onQuit = std::move(onQuit);
}

void Game::SetBackground(const sf::Texture& texture)
{
	_background.setTexture(&texture);
	_background.setSize(sf::Vector2f(texture.getSize()));
	_background.setPosition(0, 0);
}