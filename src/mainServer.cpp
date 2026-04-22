#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "login/login.hpp"
#include "server/server.hpp"
#include "helpers/helpers.hpp"
#include <sodium.h>
#include <windows.h>

#ifdef _WIN32
#pragma message("_WIN32 is defined")
#else
#pragma message("_WIN32 is NOT defined")
#endif

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
	serverPublicKey.resize(crypto_box_PUBLICKEYBYTES, 0);
	serverPrivateKey.resize(crypto_box_SECRETKEYBYTES, 0);
	crypto_box_keypair((unsigned char*)serverPublicKey.c_str(), (unsigned char*)serverPrivateKey.c_str());
	std::cout << "serverPublicKey: " << serverPublicKey << std::endl;
	std::cout << "serverPublicKey.size(): " << serverPublicKey.size() << std::endl;

	std::vector<std::string> chatMessages;

	while (true) {
		Networking::ClientSocket socket = serverSocket.accept();
		if(socket.isValid())
		{
			std::cout << "accepted new client!" << std::endl;
			Server::Client client;
			client.socket = std::move(socket);
			client.serverPublicKey = serverPublicKey;
			client.serverPrivateKey = serverPrivateKey;
			clients.push_back(std::move(client));
		}

		for(size_t i = 0; i < clients.size(); ++i)
		{
			std::cout << "client " << i << std::endl;
			Server::Client &client = clients[i];
			if (!client.socket.isValid())
			{
				std::cout << "Client connection dropped" << std::endl;
				clients.erase(clients.begin() + i);
				continue;
			}
			std::string message = client.recv();
			std::cout << message.size() << std::endl;
			if(message.size() == 0)
				continue;
			switch (message[0])
			{
			case Networking::MessageTypes::SetSession: {
				client.session = std::stoi(message.substr(1));
				std::cout << "client setting session with ID of " << client.session << std::endl;
				break;
			}
			case Networking::MessageTypes::Login: {
				size_t middle = message.find('\n', 1);
				std::string username = message.substr(1, middle - 1);
				std::cout << "client logging in with username of " << username << std::endl;
				std::string password = message.substr(middle + 1);
				if (users.login(username, password))
				{
					std::cout << "successful login!" << std::endl;
					int userID = users.findID(username);
					int sessionID = sessions.startSession(userID);
					client.session = sessionID;
					std::string reply = std::to_string(Networking::MessageTypes::Login) + std::to_string(sessionID);
					client.send(reply);
				}
				else {
					std::cout << "login failed!" << std::endl;
					std::string reply = std::to_string(Networking::MessageTypes::Login) + std::to_string(-1);
					client.send(reply);
				}
				break;
			}
			case Networking::MessageTypes::Logout: {
				std::cout << "client with session ID " << client.session << " logging out" << std::endl;
				sessions.endSession(client.session);
				break;
			}
			case Networking::MessageTypes::SendChatMessage: {
				std::cout << "receiving chat message" << std::endl;
				std::string msg = message.substr(1);
				size_t endOfID = msg.find(':');
				int sessionID = std::stoi(msg.substr(0, endOfID - 1));
				int userID = sessions.getUserID(sessionID);
				if (userID >= 0) {
					std::string username = users.findUsername(userID);
					std::string chatMessage = msg.substr(endOfID + 1);
					std::string savedMessage = message[0] + username + ": " + chatMessage;
					std::cout << savedMessage << std::endl;
					chatMessages.push_back(savedMessage);
					client.send(savedMessage);
				}
				else {
					std::cout << "Invalid session ID for chat message sent request!" << std::endl;
					client.send(message[0] + "Invalid session ID");
				}
				break;
			}
			case Networking::MessageTypes::GetChatMessages: {
				std::cout << "Request for chat messages" << std::endl;
				int sessionID = std::stoi(message.substr(1));
				int userID = sessions.getUserID(sessionID);
				if (userID >= 0) {
					std::cout << "Chat messages request has valid session ID" << std::endl;
					std::string msg;
					msg += (char)Networking::MessageTypes::GetChatMessages;
					for (auto chatMsg : chatMessages)
						msg += chatMsg + "\n\n";
					client.send(msg);
				}
				else {
					std::cout << "Invalid session ID for chat messages request" << std::endl;
					client.send(message[0] + "Invalid session ID");
				}
				break;
			}
			case Networking::MessageTypes::GetServerName: {
				std::cout << "Request for server name" << std::endl;
				std::string servername = Helpers::loadFromFile("servername.txt");
				client.send(message[0] + servername);
				break;
			}
			default: {
				std::cout << "Unrecognized message type of " + (int)message[0] << std::endl;
				break;
			}
			}
		}
		//Sleep(1000);
	}

	Networking::winSockCleanup();

	return 0;
}