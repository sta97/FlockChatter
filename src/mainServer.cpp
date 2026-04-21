#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "login/login.hpp"
#include "server/server.hpp"
#include "helpers/helpers.hpp"

int main() {
	login::initSodium();
	login::UserDatabase users;
	login::SessionDatabase sessions;

	Server::checkServername();

	Networking::initWinSock();
	Networking::ServerSocket serverSocket("8000");

	std::cout << "listening at port 8000" << std::endl;

	std::vector<Server::Client> clients;

	std::string serverPublicKey;
	std::string serverPrivateKey;

	std::vector<std::string> chatMessages;

	while (true) {
		Networking::ClientSocket socket = serverSocket.accept();
		if(socket.isValid())
		{
			Server::Client client;
			client.socket = socket;
			client.serverPublicKey = serverPublicKey;
			client.serverPrivateKey = serverPrivateKey;
			clients.push_back(client);
		}

		for(auto client : clients)
		{
			std::string message = client.recv();
			if(message.size() == 0)
				continue;
			switch (message[0])
			{
			case Networking::MessageTypes::SetSession:
				client.session = std::stoi(message.substr(1));
				break;
			case Networking::MessageTypes::Login:
				size_t middle = message.find('\n',1);
				std::string username = message.substr(1, middle-1);
				std::string password = message.substr(middle+1);
				if (users.login(username, password))
				{
					int sessionID = sessions.startSession(userID);
					client.session = sessionID;
					std::string reply = std::to_string(Networking::MessageTypes::Login) + std::to_string(sessionID);
					client.send(reply);
				}
				break;
			case Networking::MessageTypes::Logout:
				sessions.endSession(client.session);
				break;
			case Networking::MessageTypes::SendChatMessage:
				std::string msg = message.substr(1);
				size_t endOfID = msg.find(':');
				int sessionID = std::stoi(msg.substr(0, endOfID-1));
				int userID = sessions.getUserID(sessionID);
				if(userID >= 0) {
					std::string username = users.findUsername(userID);
					std::string chatMessage = msg.substr(endOfID+1);
					std::string savedMessage = message[0] + username + ": " + chatMessage;
					chatMessages.push_back(savedMessage);
					client.send(savedMessage);
				} else {
					client.send(message[0] + "Invalid session ID");
				}
				break;
			case Networking::MessageTypes::GetChatMessages:
				int sessionID = std::stoi(message.substr(1));
				int userID = sessions.getUserID(sessionID);
				if(userID >= 0) {
					std::string msg;
					msg += (char)Networking::MessageTypes::GetChatMessages;
					for(auto chatMsg : chatMessages)
						msg += chatMsg + "\n\n";
					client.send(msg);
				} else {
					client.send(message[0] + "Invalid session ID");
				}
				break;
			default:
				throw std::runtime_error("Unrecognized message type of " + (int)message[0]);
			}
		}
	}

	Networking::winSockCleanup();

	return 0;
}