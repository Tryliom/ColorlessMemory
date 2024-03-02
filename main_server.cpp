#include "server.h"
#include "logger.h"

int main()
{
	Server server;

	// Bind the listener to a port
	if (!server.Bind(PORT))
	{
		LOG_ERROR("Could not bind listener to port");
		return EXIT_FAILURE;
	}

	LOG("Server is listening to port " << PORT);

	server.StartThreads();

	while (server.running)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			server.running = false;
			break;
		}
		else
		{
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = input;

			server.SendMessageToAllClients(messageSent);
		}
	}

	return EXIT_SUCCESS;
}