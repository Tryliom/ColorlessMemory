#pragma once

#include <SFML/Network.hpp>

#include <string>

struct Message
{
  std::string content;
};

sf::Packet& operator <<(sf::Packet& packet, const Message& message);
sf::Packet& operator >>(sf::Packet& packet, Message& message);