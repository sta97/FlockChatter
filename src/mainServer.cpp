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

	std::vector<Networking::ClientSocket> clients;

	while (true) {
		Networking::ClientSocket socket = serverSocket.accept();

		std::string message = socket.recv();
		//std::cout << "received:" << std::endl << message << std::endl << std::endl;
		std::string path = http::getPath(message);
		std::cout << "path: " << path << std::endl << std::endl;
		std::vector<std::pair<std::string, std::string>> cookies = http::parseCookies(message);
		if (path == "/") {
			std::string response = indexLoggedin;
			if (cookies.size() == 1) {
				if (cookies[0].first == "session" && cookies[0].second != "") {
					std::string user = users.findUsername(sessions.getUserID(std::stoi(cookies[0].second)));
					size_t usernameStart = indexLoggedin.find("<USER>");
					response.replace(usernameStart, 6, user);
					size_t servernameLoc = response.find("<SERVERNAME>");
					std::string servername = loadFromFile("servername.txt");
					response.replace(servernameLoc, 12, servername);
					response = http::createResponse(response, "text/html; charset=utf-8");
				}
				else {
					response = index;
					size_t servernameLoc = response.find("<SERVERNAME>");
					std::string servername = loadFromFile("servername.txt");
					response.replace(servernameLoc, 12, servername);
					response = http::createResponse(response, "text/html; charset=utf-8");
				}
			}
			else
			{
				response = index;
				size_t servernameLoc = response.find("<SERVERNAME>");
				std::string servername = loadFromFile("servername.txt");
				response.replace(servernameLoc, 12, servername);
				response = http::createResponse(response, "text/html; charset=utf-8");
			}
			socket.send(response);
		}
		else if (path == "/audioTest")
			socket.send(responseAudioTest);
		else if (path == "/sendAudio" && http::getBody(message).size() > 0)
			responseAudio = http::createResponse(http::getBody(message), "application/octet-stream");
		else if (path == "/getAudio") {
			socket.send(responseAudio);
			responseAudio = http::createResponse("", "application/octet-stream");
		}
		else if (path == "/favicon.ico")
			socket.send(responseFavicon);
		else if (path == "/image.png")
			socket.send(responseImage);
		else if (path == "/login") {
			std::string response;
			std::vector<std::pair<std::string, std::string>> loginData = http::parsePostBody(message);
			std::vector<std::pair<std::string, std::string>> setCookies = {};
			if (loginData.size() == 2) {
				if (loginData[0].first == "username" && loginData[1].first == "password") {
					if (users.login(loginData[0].second, loginData[1].second)) {
						int userID = users.findID(loginData[0].second);
						response = "<a href=\"/\">Home</a> <br /> logged in as " + loginData[0].second + " with user ID " + std::to_string(userID);
						int sessionID = sessions.startSession(userID);
						setCookies.push_back(std::make_pair("session", std::to_string(sessionID)));
					}
					else
						response = "<a href=\"/\">Home</a> <br /> invalid username or password";
				}
				else
					response = "<a href=\"/\">Home</a> <br /> Invalid login parameters";
			}
			else {
				response = "<a href=\"/\">Home</a> <br /> Invalid login parameters";
			}
			response = http::createResponse(response, "text/html; charset=utf-8", setCookies);
			socket.send(response);
		}
		else if (path == "/logout") {
			sessions.endSession(std::stoi(cookies[0].second));
			socket.send(responseLogout);
		}
		else
			socket.send(response404);
	}

	Networking::winSockCleanup();

	return 0;
}