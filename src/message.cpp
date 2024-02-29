#include "message.h"

sf::Packet& operator <<(sf::Packet& packet, const Message& message)
{
  return packet << message.content;
}

sf::Packet& operator >>(sf::Packet& packet, Message& message)
{
  return packet >> message.content;
}