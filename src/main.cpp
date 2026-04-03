#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "http/http.hpp"
#include "login/login.hpp"

#pragma comment(lib, "Ws2_32.lib")

std::string loadFromFile(std::string filename) {
    std::ifstream file(filename, std::ios::binary);  // Open the file

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open " + filename);
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));

    file.close();

    return fileContents;
}

int main() {
    login::UserDatabase users;

    Networking::initWinSock();
    Networking::ServerSocket serverSocket("8000");

    std::cout << "listening at http://127.0.0.1:8000/" << std::endl;

    std::string index = loadFromFile("assets/index.html");
    std::string icon = loadFromFile("assets/favicon.ico");
    std::string image = loadFromFile("assets/image.png");

    std::string responseIndex = http::createResponse(index, "text/html; charset=utf-8");
    std::string responseFavicon = http::createResponse(icon, "image/x-icon");
    std::string responseImage = http::createResponse(image, "image/png");
    std::string response404 = "HTTP/1.1 404 Not Found";

    while (true) {
        Networking::ClientSocket socket = serverSocket.accept();
        std::string message = socket.recv();
        std::cout << "received:" << std::endl << message << std::endl << std::endl;
        std::string path = http::getPath(message);
        std::cout << "path: " << path << std::endl << std::endl;
        if (path == "/")
            socket.send(responseIndex);
        else if (path == "/favicon.ico")
            socket.send(responseFavicon);
        else if (path == "/image.png")
            socket.send(responseImage);
        else if (path == "/login") {
            std::string response;
            std::vector<std::pair<std::string, std::string>> loginData = http::parsePostBody(message);
            if (loginData.size() == 2) {
                response += "<a href=\"/\">Home</a>";
                response += "<br />\n";
                response += loginData[0].first + ": " + loginData[0].second + "\n";
                response += "<br />\n";
                response += loginData[1].first + ": " + loginData[1].second + "\n";
            }
            else {
                response = "<a href=\"/\">Home</a> <br /> Invalid login parameters";
            }
            response = http::createResponse(response, "text/html; charset=utf-8");
            socket.send(response);
        }
        else
            socket.send(response404);
    }

    Networking::winSockCleanup();

    return 0;
}