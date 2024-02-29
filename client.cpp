#include "message.h"

#include <SFML/Network.hpp>

#include <iostream>

int main()
{
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("localhost", 12345);
	if (status != sf::Socket::Done)
	{
		// error...
		std::cerr << "Could not connect to server!\n";
		return EXIT_FAILURE;
	}
	Message messageSent;
	std::cin >> messageSent.content;

	sf::Packet request;
	request << messageSent;


	if (socket.send(request) != sf::Socket::Done)
	{
		std::cerr << "Could not send message to server \n";
		return EXIT_FAILURE;
	}
	sf::Packet answer;
	if (socket.receive(answer) != sf::Socket::Done)
	{
		std::cerr << "Could not receive answer from server\n";
	}
	Message answerMsg;
	answer >> answerMsg;

	std::cout << answerMsg.content << '\n';
	return EXIT_SUCCESS;
}